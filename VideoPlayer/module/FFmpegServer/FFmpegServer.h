#ifndef FFMPEGSERVER_H
#define FFMPEGSERVER_H
/**************************************************\
 * 类名: FFmpegServer
 * 描述: 视频播放类
 * 信号：
 * 函数：
 * 备注：
 * 作者：wqj  QQ:1107097641
 * 博客地址：
 *      日期          版本号                描述
 * 2024年02年29日     v1.0.0         ffmpeg模块封装空类
\**************************************************/
#include "qaudiooutputstrategy.h"
#include "sdlaudiooutputstrategy.h"
#include "audiooutputcontext.h"
#include "ffmpegsynclist.h"
#include "ffmpegHead.h"

#include <QObject>
#include <QString>
#include <QThread>
#include <QImage>
#include <queue>

#define DEBUG(msg) \
    (qDebug() << QString("[%1][%2:%3]%4").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz")).arg(__FILE__).arg(__LINE__).arg(msg));

//#define SDL_AUDIO_BUFFER_SIZE 1024  //音频缓冲区
//#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

//#define MAX_AUDIO_SIZE (50 * 20)
//#define MAX_VIDEO_SIZE (25 * 20)

enum PlaybackStatus {
    PLAY,
    PAUSE,
    EXIT
};

class FFmpegServer : public QThread
{
    Q_OBJECT
public:

public:
    explicit FFmpegServer(QObject *parent = nullptr);

public:
    inline static QString getAvcodecConfiguration()
    {
        return QString(avcodec_configuration());
    }

    //解码同步线程定义成友元类
    friend class FFmpegSyncList;

    void versionInfo();


    void setPlayUrl(const QString &url);
    int getLength();        //获取帧数

    //初始化
    int initInput();

    //释放缓存
    void clear();
    void stopPlay();

    int test();


    //音视频数据包的解码处理
    void transVideoPacket(AVPacket *packet);
    void transAudioPacket(AVPacket *packet);
    //解码音视频
    void decodeVideoPacket(AVPacket *packet);
    void decodeAudioPacket(AVPacket *packet);


    //播放延时计算
    qint64 getPacketPts(AVPacket *packet);
    qint64 calcDelayTime(AVFormatContext *formatCtx, AVPacket *packet);
    void timeDelay(AVFormatContext *formatCtx, AVPacket *packet);




public slots:
    //播放控制接口
    //开始播放
    //设置暂停
    void setPause();
    //设置进度条
    void setProgress(int value);
    //设置音量
    void setVolume(int value);
    //设置静音
    void setSoundOff();


private:
    void setOption();
    int setInput();
    int setVideo();
    int setAudio();


    //ffmpeg
    //媒体文件或媒体流的构成和基本信息
    AVFormatContext* pFormatCtx = NULL;
    //字典集合，键值对，用于配置相关信息
    AVDictionary* options = NULL;

    //寻找流
    int videoStreamIndex = -1;
    int audioStreamIndex = -1;
    //一个描述编解码器上下文的数据结构,包含了众多编解码器需要的参数信息
    AVCodecContext  *videoCodecContext = NULL;
    AVCodecContext  *audioCodecContext = NULL;

    //存储编解码器信息的结构体
    AVCodec* videoCodec = NULL;
    AVCodec* audioCodec  = NULL;


    //包缓存
    uint8_t *rgbBuffer = NULL;
    AVPacket *packet = NULL;
    //转换上下文
    SwsContext *swsCtx = NULL;
    AVFrame *pFrame = NULL;
    AVFrame *pFrameRGB = NULL;
    int width = 0;
    int height = 0;
    int got_picture = 0;

    qint64 startPrecessTime;          //数据处理开始时间
    qint64 pauseTime;                 //点击暂停时间
    qint64 videoFirstPts;             //首个视频包的pts
    qint64 audioFirstPts;             //首个音频包的pts
    uint8_t *videoData;               //解码后图片数据
    uint8_t *audioData;               //解码后音频数据

    AVPacket *avPacket = NULL;               //包对象
    AVFrame *videoFrame = NULL;              //视频帧对象
    AVFrame *videoFrameDst = NULL;           //视频帧转换后的对象
    AVFrame *audioFrame = NULL;              //音频帧对象
    SwsContext *videoSwsCtx = NULL;          //视频数据转换对象
    SwrContext *audioSwrCtx = NULL;          //音频数据转换对象重采样

    FFmpegSyncList *videoSync;        //视频同步队列
    FFmpegSyncList *audioSync;        //音频同步队列



    double frameRate;                 //帧率
    int duration;                     //时长 单位秒
    int frameFinish;                  //一帧完成
    int videoWidth;                   //视频宽度
    int videoHeight;                  //视频高度
    int volume;                       //音量


    volatile bool isRun;              //线程开关标志位
    volatile bool isPause;            //播放暂停标志位
    volatile bool isSpeed;            //倍速播放标志位
    volatile bool isSendPosition;     //返回播放进度标志位

    bool isNetUrl;                    //网络地址
    bool isUsbCamera;                 //USB设备
    bool isPlayAudio;                 //启用音频
    bool audioDeviceOk;               //音频设备正常
    bool switchSpeed;                 //切换速度
    bool enableFilter;                //开启水印功能
    int  lastSpeedLevel;              //上次倍速级别

    QString playUrl;                  //播放地址
    int frameTime;                    //当前帧播放时间
    volatile double playSpeed;        //解码间隔时间 pts方式
    volatile double lastPlaySpeed;    //解码间隔时间 pts方式



    //queue
    std::queue<AVFrame*> audioFrameQueue; // 音频帧队列
    std::mutex audioFrameMutex; // 音频帧锁
    std::condition_variable audioFrameCond; // 音频帧条件变量

    std::queue<AVFrame*> videoFrameQueue; // 视频帧队列
    std::mutex videoFrameMutex; // 视频帧锁
    std::condition_variable videoFrameCond; // 视频帧条件变量

    std::queue<AVFrame*> audioPcmFrameQueue; // PCM音频帧队列
    std::mutex audioPcmFrameMutex; // PCM音频帧锁
    std::condition_variable audioPcmFrameCond; // PCM音频帧条件变量

    QString lastError;



    PlaybackStatus status = PLAY;

    //Audio
//    SDLAudioOutputStrategy m_SDLAudioOutputStrategy;
    QAudioOutputStrategy m_QAudioOutputStrategy;
//    AudioOutputContext context;


protected:
    void run() override;

signals:
    //显示图像
    void setImage_signal(QImage image);

    //设置窗口显示进度
    void sendPlayPosition(int sec);


};

#endif // FFMPEGSERVER_H
