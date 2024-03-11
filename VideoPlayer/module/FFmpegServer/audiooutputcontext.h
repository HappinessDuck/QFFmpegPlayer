#ifndef AUDIOOUTPUTCONTEXT_H
#define AUDIOOUTPUTCONTEXT_H
#include "audiooutputstrategy.h"

class AudioOutputContext
{
public:
    AudioOutputContext();
    ~AudioOutputContext()
    {
        delete this->strategy_;
    }

    void set_strategy(AudioOutputStrategy* strategy)
    {
        delete this->strategy_;
        this->strategy_ = strategy;
    }

    bool init(int sampleRate, int sampleSize, int channelCount)
    {
        bool ret = false;
        ret = this->strategy_->init(sampleRate, sampleSize, channelCount);
        ret = this->strategy_->play();
        return ret;
    }

    void write(char *data, int datasize)
    {
        this->strategy_->write(data, datasize);
    }


private:
    AudioOutputStrategy* strategy_;

};

#endif // AUDIOOUTPUTCONTEXT_H
