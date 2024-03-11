#ifndef SDLAUDIOOUTPUTSTRATEGY_H
#define SDLAUDIOOUTPUTSTRATEGY_H
#include "audiooutputstrategy.h"
#include "stdio.h"
extern "C" {
#include <SDL.h>
//    #include <SDL_audio.h>
//    #include <SDL_types.h>
//    #include <SDL_name.h>
//    #include <SDL_main.h>
//    #include <SDL_config.h>
#undef main
}

//SDL使用类
class SDLAudioOutputStrategy : public AudioOutputStrategy
{
public:
    SDLAudioOutputStrategy();

    bool init(int sampleRate, int sampleSize, int channelCount) override
    {
        // SDL音频设备初始化的具体实现
        if (SDL_Init(SDL_INIT_AUDIO) < 0)
        {
            printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        }
        SDL_AudioSpec want, have;
        SDL_zero(want);
        want.freq = 44100;
        want.format = AUDIO_S16SYS;
        want.channels = 2;
        want.samples = 4096;

        dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
        return true;

    }

    bool play() override
    {
        // SDL音频设备开始播放的具体实现
        SDL_PauseAudio(0);
        SDL_PauseAudioDevice(dev, 0);

        return true;
    }

    bool stop() override
    {
        // SDL音频设备停止播放的具体实现
        SDL_PauseAudioDevice(dev, 1);

        return true;
    }

    bool write(char *data, int datasize) override
    {
        return true;
    }

private:
    SDL_AudioDeviceID dev;


};

#endif // SDLAUDIOOUTPUTSTRATEGY_H
