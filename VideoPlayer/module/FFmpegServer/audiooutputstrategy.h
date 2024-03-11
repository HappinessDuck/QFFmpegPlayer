#ifndef AUDIOOUTPUTSTRATEGY_H
#define AUDIOOUTPUTSTRATEGY_H

//音频抽象类
class AudioOutputStrategy
{
public:
    AudioOutputStrategy();
    virtual ~AudioOutputStrategy() = default;
    virtual bool init(int sampleRate, int sampleSize, int channelCount) = 0;
    virtual bool play() = 0;
    virtual bool stop() = 0;
    virtual bool write(char *data, int datasize) = 0;
};

#endif // AUDIOOUTPUTSTRATEGY_H
