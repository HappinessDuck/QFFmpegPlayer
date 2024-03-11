#ifndef FFMPEGSYNCLIST_H
#define FFMPEGSYNCLIST_H

/************************************************************************
  *Copyright(C) ZXT 1174110975@qq.com
  *FileName: ffmpegsynclist.h
  *Author: 周新童
  *Date: 2022-01-11
  *Description: 1.将解码后的音视频avPacket存入队列，不断出队取数据渲染或者播放声音
                2.视频的渲染，以及音频送入声卡播放，互补干扰，可以得到较好的同步效果
*************************************************************************/

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QList>
#include "ffmpegHead.h"


class FFmpegServer;

class FFmpegSyncList : public QThread
{
public:
    explicit FFmpegSyncList(QObject *parent = nullptr);

    //设置主解码线程
    void setThread(FFmpegServer *thread);

    //数据packet队列操作
    void appendPacket(AVPacket *packet);
    void clearPacketList();
    int  getPacketListSize();

    //设置数据类型
    void setType(int type);

    //线程的开启、关闭
    void startRun();
    void stopRun();

protected:
    void run();

private:
    volatile bool isRun;            //线程停止标志位
    FFmpegServer *thread;           //解码主线程
    QList<AVPacket *> packetList;   //数据包队列
    QMutex mutex;                   //数据锁
    int type;                       //数据类型 视音频或其它
};

#endif // FFMPEGSYNCLIST_H
