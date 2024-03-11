#ifndef QAUDIOOUTPUTSTRATEGY_H
#define QAUDIOOUTPUTSTRATEGY_H
#include "audiooutputstrategy.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <QIODevice>
#include <QDebug>
//QT音频使用类
class QAudioOutputStrategy : public AudioOutputStrategy
{
public:
    QAudioOutputStrategy();
    bool init(int sampleRate, int sampleSize, int channelCount) override
    {
        // QT音频设备初始化的具体实现
        //输入信息
        QAudioFormat format;
        format.setSampleRate(sampleRate);//采样率
        format.setSampleSize(sampleSize*8);		//32位 =>16位  转换？？ 音频重采样
        format.setChannelCount(channelCount);     //通道数    2个  左右
        format.setCodec("audio/pcm");      //通用格式
        format.setByteOrder(QAudioFormat::LittleEndian);// 字节序，采用默认小端（低位低字节，高位高字节）
//        format.setSampleType(QAudioFormat::UnSignedInt);//  样本类型  会出现噪音
        format.setSampleType(QAudioFormat::SignedInt);//  样本类型

        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        bool res = info.isFormatSupported(format);
        if(res)
        {
            audioOutput = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), format);

            //设置下缓存不然部分文件播放音频一卡卡  太大可能会导致崩溃分配内存失败
            audioOutput->setBufferSize(40960);
            audioOutput->setVolume(1.0);
            audioDevice = audioOutput->start();
        }
        else
        {
            qDebug() << "Raw audio format not supported, cannot play audio.";
        }

        return res;
    }

    bool play() override
    {
        // QT音频设备开始播放的具体实现
        return true;
    }

    bool stop() override
    {
        // QT音频设备停止播放的具体实现
        return true;
    }

    bool write(char *data, int datasize) override
    {
        if (!data || datasize <= 0)
            return false;

        if (audioDevice)
        {
            audioDevice->write(data, datasize);
            return true;
        }
        return false;
    }

    bool setVolume(double value)
    {
        if (audioOutput)
        {
            audioOutput->setVolume(value);
            return true;
        }
        return false;
    }


private:
    QAudioOutput* audioOutput = nullptr;
    QIODevice* audioDevice = nullptr;

};

#endif // QAUDIOOUTPUTSTRATEGY_H
