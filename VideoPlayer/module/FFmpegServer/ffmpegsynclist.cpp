#include "ffmpegsynclist.h"
#include "FFmpegServer.h"

FFmpegSyncList::FFmpegSyncList(QObject *parent) : QThread(parent)
{
    isRun = false;
    type = 0;
    thread = nullptr;
}

void FFmpegSyncList::setThread(FFmpegServer *thread)
{
    this->thread = thread;
}

void FFmpegSyncList::appendPacket(AVPacket *packet)
{
    QMutexLocker locker(&mutex);
    packetList.append(packet);
}

void FFmpegSyncList::clearPacketList()
{
    QMutexLocker locker(&mutex);

    //释放未处理的packet
    foreach (AVPacket *packet, packetList)
    {
        av_packet_unref(packet);;
    }

    packetList.clear();
}

int FFmpegSyncList::getPacketListSize()
{
    return packetList.size();
}

void FFmpegSyncList::setType(int type)
{
    this->type = type;
}

void FFmpegSyncList::startRun()
{
    isRun = true;
    this->start();
}

void FFmpegSyncList::stopRun()
{
    isRun = false;
}

void FFmpegSyncList::run()
{
    while(isRun)
    {
        //暂停状态或者队列中没有帧则不处理
        if (thread->isPause || packetList.size() <= 0)
        {
            msleep(1);
            continue;
        }

        //读取队列首个包
        mutex.lock();
        AVPacket *packet = packetList.first();
        mutex.unlock();

        //计算当前帧显示时间 均与外部时钟同步
        thread->timeDelay(thread->pFormatCtx,packet);

        //1表示视频 2表示音频
        if (type == 1)
        {
            thread->decodeVideoPacket(packet);
        }
        else if (type == 2)
        {
            thread->decodeAudioPacket(packet);
        }

        //释放资源并从队列移除
        av_packet_unref(packet);
        if(!packetList.isEmpty())
        {
            mutex.lock();
            packetList.removeFirst();
            mutex.unlock();
        }
    }

    msleep(1);
    isRun = false;
    clearPacketList();
}

