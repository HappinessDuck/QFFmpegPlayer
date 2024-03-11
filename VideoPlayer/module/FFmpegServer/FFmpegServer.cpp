#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

#include "FFmpegServer.h"
#include "stdio.h"
#include <QDebug>
#include <thread>

FFmpegServer::FFmpegServer(QObject *parent): QThread(parent)
{
    versionInfo();
    //注册
    avdevice_register_all();
    //初始化网络库
    avformat_network_init();

    //设置音频输出方式
//    context.set_strategy(&m_QAudioOutputStrategy);

    // 初始化音频相关参数
    audioFrameQueue = std::queue<AVFrame*>(); // 音频帧队列
    // 初始化视频相关参数
    videoFrameQueue = std::queue<AVFrame*>(); // 视频帧队列

    duration=0;                     //时长 单位秒
    frameFinish=0;                  //一帧完成
    videoWidth=0;                   //视频宽度
    videoHeight=0;                 //视频高度
    volume=0;                      //音量

    isRun=false;              //线程开关标志位
    isPause=false;            //播放暂停标志位
    isSpeed=false;            //倍速播放标志位
    isSendPosition=false;     //返回播放进度标志位

    isNetUrl=false;                    //网络地址
    isUsbCamera=false;              //USB设备
    isPlayAudio=true;                 //启用音频
    audioDeviceOk=false;               //音频设备正常
    switchSpeed=false;                 //切换速度
    enableFilter=false;                //开启水印功能
    lastSpeedLevel=1;              //上次倍速级别



    videoSync = new FFmpegSyncList(this);
    videoSync->setType(1);
    audioSync = new FFmpegSyncList(this);
    audioSync->setType(2);
}

void FFmpegServer::versionInfo()
{
    qDebug() << "ffmpeg version:"<< av_version_info();
//    qDebug() << "ffmpeg configuration:"<< avcodec_configuration();
}

void FFmpegServer::setPlayUrl(const QString &url)
{
    playUrl = url;
}

void FFmpegServer::setOption()
{
    //设置缓存大小,1080p可将值调大
    av_dict_set(&opt, "buffer_size", "8192000", 0);
    //以tcp方式打开,如果以udp方式打开将tcp替换为udp
    av_dict_set(&opt, "rtsp_transport", "tcp", 0);
    //设置超时断开连接时间,单位微秒,3000000表示3秒
    av_dict_set(&opt, "stimeout", "3000000", 0);
    //设置最大时延,单位微秒,1000000表示1秒
    av_dict_set(&opt, "max_delay", "1000000", 0);
    //自动开启线程数
    av_dict_set(&opt, "threads", "auto", 0);

    //设置编码格式
    //av_dict_set(&pOptions, "input_format", "mjpeg", 0);
    //设置分辨率
    //av_dict_set(&pOptions,"video_size","800x600",0);
    //设置帧率
    //av_dict_set(&pOptions,"framerate","30",0);
}

int FFmpegServer::setInput()
{
    //分配解复用器上下文
    pFormatCtx = avformat_alloc_context();

    AVInputFormat *iFmt = NULL;
    if(isUsbCamera)
    {
        iFmt = av_find_input_format("dshow");
    }
    //AVInputFormat *ifmt = av_find_input_format("vfwcap"); // vfwcap方式打开


    //打开输入
    //摄像头--本地视频--网络流视频
    int ret = avformat_open_input(&pFormatCtx, playUrl.toStdString().data(), iFmt, &opt);
    if(ret != 0)
    {
        lastError = "Couldn't open input stream." + playUrl;
        return -1;
    }
    //读取媒体的部分数据包以获取码流信息
    ret = avformat_find_stream_info(pFormatCtx,NULL);
    if(ret < 0)
    {
        lastError = "Couldn't find stream information.";
        return -1;
    }
    return 0;
}

int FFmpegServer::setVideo()
{
    //获取固定长度失败，如直播流无时长信息，则不发送当前播放位置
    duration = pFormatCtx->duration / AV_TIME_BASE;
//    qDebug() << "video duration=" << duration;
    if(duration < 0)
    {
        isSendPosition = false;
    }

    // 查找视频流id
    videoStreamIndex = av_find_best_stream(pFormatCtx,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,NULL);
    if(videoStreamIndex < 0)
    {
        lastError = "Couldn't find a video stream.";
        return -1;
    }
    videoFirstPts = pFormatCtx->streams[videoStreamIndex]->start_time;
    frameRate = av_q2d(pFormatCtx->streams[videoStreamIndex]->r_frame_rate);
#if 0
    qDebug() << "video width=" << pFormatCtx->streams[videoStreamIndex]->codecpar->width;
    qDebug() << "video height=" << pFormatCtx->streams[videoStreamIndex]->codecpar->height;
    //帧率 fps 分数转换
    qDebug() << "video fps = " << frameRate;

    int totalMs = pFormatCtx->duration / (AV_TIME_BASE / 1000);
    qDebug() << "totalMs = " << totalMs;
    //    av_dump_format(pFormatCtx, NULL, path, 0);
#endif
    //////////////////////////////////////////////////////////
    //获取音频解码器和视频解码器
    videoCodecContext = pFormatCtx->streams[videoStreamIndex]->codec;
    videoCodec = avcodec_find_decoder(videoCodecContext->codec_id);

    if (!videoCodecContext) {
        lastError = "video codec not found" ;
        return -1;
    }
    //打开视频解码器 设置加速解码
    videoCodecContext->lowres = videoCodec->max_lowres;
    videoCodecContext->flags2 |= AV_CODEC_FLAG2_FAST;


    //打开编解码器
    if (avcodec_open2(videoCodecContext, videoCodec, NULL) < 0)
    {
        lastError = "Could not open codec.";
        return -1;
    }

    //获取分辨率大小
    videoWidth = videoCodecContext->width;
    videoHeight = videoCodecContext->height;

    if (videoWidth == 0 || videoHeight == 0)
    {
        lastError = "find width height error";
        return -1;
    }

    //分配对象内存 退出后释放
    videoFrame = av_frame_alloc();
    videoFrameDst = av_frame_alloc();

    int byte = avpicture_get_size(AV_PIX_FMT_RGB32, videoWidth, videoHeight);
    videoData = (uint8_t *)av_malloc(byte * sizeof(uint8_t));
    if (!videoData)
    {
        av_free(videoData);
        qDebug() << "videoData malloc fail";
        return false;
    }

    //定义像素格式
    AVPixelFormat srcFormat = videoCodecContext->pix_fmt;  //AV_PIX_FMT_YUV420P
    AVPixelFormat dstFormat = AV_PIX_FMT_RGB32;

    //默认最快速度的解码采用的SWS_FAST_BILINEAR参数,可能会丢失部分图片数据,可以自行更改成其他参数
    int flags = SWS_BILINEAR;

    //开辟缓存存储一帧数据到videoData,设置转换  videoFrameDst->data指向videoData
    av_image_fill_arrays(videoFrameDst->data, videoFrameDst->linesize, videoData, dstFormat, videoWidth, videoHeight, 1);
    videoSwsCtx = sws_getContext(videoWidth, videoHeight, srcFormat, videoWidth, videoHeight, dstFormat, flags, NULL, NULL, NULL);

    QString videoInfo = QString("视频流信息 -> 索引: %1  解码: %2  时长: %3 秒  分辨率: %4*%5")
            .arg(videoStreamIndex)
            .arg(videoCodec->name)
            .arg(duration)
            .arg(videoWidth).arg(videoHeight);

    qDebug() << videoInfo;
    return 0;
}

int FFmpegServer::setAudio()
{
    // 查找音频流id
    audioStreamIndex = av_find_best_stream(pFormatCtx,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,NULL);
    //有些视频没有音频流,所以这里不视为失败
    if(audioStreamIndex < 0)
    {
        audioDeviceOk = false;
        lastError = "Couldn't find a audio stream.";
    }else
    {
#if 0
        qDebug() << "audio sample_rate = " << pFormatCtx->streams[audioStreamIndex]->codecpar->sample_rate;
        //AVSampleFormat;
        qDebug() << "audio channels = " << pFormatCtx->streams[audioStreamIndex]->codecpar->channels;
        //一帧数据？？ 单通道样本数
        qDebug() << "audio frame_size = " << pFormatCtx->streams[audioStreamIndex]->codecpar->frame_size;
        //1024 * 2 * 2 = 4096  fps = sample_rate/frame_size
#endif
        audioCodecContext = pFormatCtx->streams[audioStreamIndex]->codec;
        audioFirstPts = pFormatCtx->streams[audioStreamIndex]->start_time;

        audioCodec = avcodec_find_decoder(audioCodecContext->codec_id);
        if (!audioCodecContext) {
            lastError = "audio codec not found" ;
            return -1;
        }

        //打开编解码器
        if (avcodec_open2(audioCodecContext, audioCodec, NULL) < 0)
        {
            lastError = "Could not open codec.";
            return -1;
        }

        //初始化音频
        int sampleRate = audioCodecContext->sample_rate;
        int sampleSize = av_get_bytes_per_sample(audioCodecContext->sample_fmt)/2;
        int channelCount = audioCodecContext->channels;
        audioDeviceOk = m_QAudioOutputStrategy.init(sampleRate, sampleSize, channelCount);

        //设置音频转换
        if(audioDeviceOk)
        {
            //内存分配
            audioSwrCtx = swr_alloc();

            int64_t channelOut = AV_CH_LAYOUT_STEREO;
            int64_t channelIn = av_get_default_channel_layout(audioCodecContext->channels);
            audioSwrCtx = swr_alloc_set_opts(audioSwrCtx, channelOut, AV_SAMPLE_FMT_S16, sampleRate, channelIn, audioCodecContext->sample_fmt, sampleRate, 0, 0);
            audioDeviceOk = (swr_init(audioSwrCtx) >= 0);

            //分配音频数据内存 192000 ffplay代码值 可调节
            if (audioDeviceOk)
            {
                quint64 byte = 19200;
                audioData = (uint8_t *)av_malloc(byte * sizeof(uint8_t));
                if (!audioData)
                {
                    audioDeviceOk = false;
                    qDebug() << "audioData malloc fail";
                    av_free(audioData);
                    return false;
                }
            }
        }

        QString audioInfo = QString("音频流信息 -> 索引: %1  解码: %2  比特率: %3  声道数: %4  采样: %5")
                .arg(audioStreamIndex)
                .arg(audioCodec->name)
                .arg(pFormatCtx->bit_rate)
                .arg(audioCodecContext->channels)
                .arg(audioCodecContext->sample_rate);
        qDebug() << audioInfo;
    }
}

int FFmpegServer::getLength()
{
    return duration;
}



int FFmpegServer::initInput()
{
    //初始化参数
    setOption();

    //初始化输入
    if (setInput() != 0)
    {
        return -1;
    }

    //初始化视频
    if (setVideo() != 0)
    {
        return -1;
    }

    //初始化音频
    setAudio();


    return 0;
}



void FFmpegServer::clear()
{
//    sws_freeContext(swsCtx);
//    av_free(rgbBuffer);
//    av_free(pFrame);
//    av_free(pFrameRGB);
//    avcodec_close(videoCodecContext);// 关闭编码/解码器
//    avformat_close_input(&pFormatCtx);//关闭文件上下
}



int FFmpegServer::test()
{

    return 0;
}

void FFmpegServer::setPause()
{
    if(isPause)
    {
        //next play
        int64_t offset = av_gettime() - pauseTime;
        startPrecessTime += offset;
    }else{
        //pause play
        pauseTime = av_gettime();
    }
    isPause = !isPause;
}

void FFmpegServer::setProgress(int value)
{

}

void FFmpegServer::setVolume(int value)
{
    volume = value;
    double doubleValue = static_cast<double>(value) / 100.0;
    m_QAudioOutputStrategy.setVolume(doubleValue);
}

void FFmpegServer::setSoundOff()
{
    isPlayAudio = !isPlayAudio;
}

qint64 FFmpegServer::getPacketPts(AVPacket *packet)
{
    //有些文件(比如asf文件)取不到pts需要矫正
    int64_t pts = 0;
    if(packet->dts == AV_NOPTS_VALUE && packet->pts && packet->pts != AV_NOPTS_VALUE)
    {
        pts = packet->pts;
    }
    else if (packet->dts != AV_NOPTS_VALUE)
    {
        pts = packet->dts;
    }
    return pts;
}

qint64 FFmpegServer::calcDelayTime(AVFormatContext *formatCtx, AVPacket *packet)
{
    //播放速度处理
    int64_t pts = getPacketPts(packet);
    int64_t newPts = pts * playSpeed;
    int64_t lastPts = pts * lastPlaySpeed;

    //当前视音频帧应该显示的时刻
    AVRational time_base = formatCtx->streams[packet->stream_index]->time_base;
    AVRational time_base_q = {1, AV_TIME_BASE};
    int64_t lastPts_time = 0;
    int64_t newPts_time = av_rescale_q(newPts, time_base, time_base_q);

    //切换播放速度
    if(switchSpeed)
    {
        lastPts_time = av_rescale_q(lastPts, time_base, time_base_q);
        int64_t correction = lastPts_time - newPts_time;
        startPrecessTime += correction;

        lastPlaySpeed = playSpeed;
        switchSpeed = false;
    }

    //开始播放时刻到此刻的时间差值
    int64_t now_time = av_gettime() - startPrecessTime;

    //计算差值 进行延时或者无延时跟进
    int64_t offset_time = newPts_time - now_time;
    return offset_time;
}

void FFmpegServer::timeDelay(AVFormatContext *formatCtx, AVPacket *packet)
{
    qint64 offset_time = calcDelayTime(formatCtx, packet);
    if(offset_time>0 && offset_time < 1*1000*1000)
    {
        av_usleep(offset_time);
    }
}

void FFmpegServer::transVideoPacket(AVPacket *packet)
{
    //有些视频保存的MP4文件首帧开始的时间不是0 需要减去
    if (videoFirstPts > AV_TIME_BASE)
    {
        packet->pts -= videoFirstPts;
        packet->dts = packet->pts;
    }
    videoSync->appendPacket(av_packet_clone(packet));

}

void FFmpegServer::transAudioPacket(AVPacket *packet)
{
    //有些视频保存的MP4文件首帧开始的时间不是0 需要减去
    if (audioFirstPts > AV_TIME_BASE)
    {
        packet->pts -= audioFirstPts;
        packet->dts = packet->pts;
    }

    audioSync->appendPacket(av_packet_clone(packet));

}

void FFmpegServer::decodeVideoPacket(AVPacket *packet)
{
    int ret = avcodec_send_packet(videoCodecContext, packet);
    while (ret >= 0)
    {
        AVFrame* frame = av_frame_alloc();
        ret = avcodec_receive_frame(videoCodecContext, frame);
        if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
        {
            av_frame_free(&frame);
            break;
        }

        //send signal, show frame
        //将解码后的frame数据转换
        sws_scale(videoSwsCtx, (const uint8_t *const *)frame->data, frame->linesize, 0, videoHeight, videoFrameDst->data, videoFrameDst->linesize);

        //构造QImage videoFrameDst->data[0]指向的就是videoData，二者等同
        QImage image((uchar *)videoData, videoWidth, videoHeight, QImage::Format_RGB32);

        if (!image.isNull())
        {
            emit setImage_signal(image);
        }

        //获取当前帧的播放时间  avPacket与videoFrame pts相同为同一时刻，避免水印影响统一使用packet的pts
        if(isSendPosition)
        {
            int curtime = packet->pts * av_q2d(pFormatCtx->streams[videoStreamIndex]->time_base);
            if(curtime != frameTime)
            {
                frameTime = curtime;
                emit sendPlayPosition(frameTime);
            }
        }

        //释放资源引用
        av_frame_unref(frame);
    }

}

void FFmpegServer::decodeAudioPacket(AVPacket *packet)
{
    //是否启用音频
    if(!isPlayAudio)
    {
        return;
    }

    //初始化设置未成功
    if(!audioDeviceOk)
    {
        return;
    }

    int ret = avcodec_send_packet(audioCodecContext, packet);
    if(ret >= 0)
    {
        audioFrame = av_frame_alloc();
        ret = avcodec_receive_frame(audioCodecContext, audioFrame);
        if (ret >= 0)
        {
            int outChannel = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            int outSize = av_samples_get_buffer_size(NULL, outChannel, audioFrame->nb_samples, AV_SAMPLE_FMT_S16, 1);

            int result = swr_convert(audioSwrCtx, &audioData, outSize, (const uint8_t **)audioFrame->data, audioFrame->nb_samples);
            if(result >= 0)
            {
                //播放音频
                m_QAudioOutputStrategy.write((char *)audioData, outSize);
            }
        }
        av_frame_unref(audioFrame);
    }
}

void FFmpegServer::run()
{
    //check isRun
    isRun = true;
    isPause = false;

    frameTime = 0;
    pauseTime = 0;
    switchSpeed = false;
    lastSpeedLevel = 1;
    lastPlaySpeed = 1;
    playSpeed = 1;

    startPrecessTime = av_gettime();

    videoSync->startRun();
    videoSync->setThread(this);
    audioSync->startRun();
    audioSync->setThread(this);

    avPacket = av_packet_alloc();

    while(isRun)
    {
        if(isPause)
        {
            msleep(50);
            continue;
        }

        //解码队列中数量过多暂停读取 值可以自行调整
        if ((videoSync->getPacketListSize() >= 50 || audioSync->getPacketListSize() >= 50))
        {
            msleep(1);
            continue;
        }

        //读取包进行解码操作
        if(av_read_frame(pFormatCtx, avPacket) >= 0)
        {
            //判断当前包是视频还是音频
            int index = avPacket->stream_index;
            if (index == videoStreamIndex)
            {
                transVideoPacket(avPacket);
            }
            else if (index == audioStreamIndex)
            {
                transAudioPacket(avPacket);
            }
        }
        else
        {
            break;
        }

        av_packet_unref(avPacket);
    }

    //free
    if(avPacket!=NULL)
    {
        av_packet_free(&avPacket);
        avPacket = NULL;
    }


    qDebug()<<"play thread end";

    //停止解码同步线程
    if (audioSync->isRunning())
    {
        audioSync->stopRun();
        audioSync->quit();
        audioSync->wait(150);
    }

    if (videoSync->isRunning())
    {
        videoSync->stopRun();
        videoSync->quit();
        videoSync->wait(150);
    }


}















