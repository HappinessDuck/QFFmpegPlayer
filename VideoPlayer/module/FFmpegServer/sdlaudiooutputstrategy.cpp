#include "sdlaudiooutputstrategy.h"
#include <QDebug>
SDLAudioOutputStrategy::SDLAudioOutputStrategy()
{
    SDL_version version;
    SDL_VERSION(&version);
    qDebug()<<"version major is"<<version.major;
    qDebug()<<"version minor is"<<version.minor;
}
