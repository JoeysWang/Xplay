//
// Created by 王越 on 2020/4/26.
//

#ifndef XPLAY_IDEMUX_H
#define XPLAY_IDEMUX_H

#include "../data/XData.h"
#include "../XThread.h"
#include "../IObserver.h"
#include "../player/PlayerState.h"

struct XParameter;

//解封装接口
class IDemux : public IObserver {

public:
    IDemux(PlayerState *playerState);

    virtual ~IDemux();

//打开文件、流媒体 http rtsp
    virtual bool open(const char *url) = 0;

    //读取一帧数据，数据由调用者清理
    virtual XData read() = 0;

    virtual XParameter getVideoParameter() = 0;

    virtual XParameter getAudioParameter() = 0;

    virtual AVStream *getAudioStream() = 0;

    virtual AVStream *getVideoStream() = 0;

    //总时长
    int totalMs = 0;
public:
    AVStream *pStream=0;
    AVFormatContext *formatContext = 0;
    int audioStreamIndex = 0;
    int videoStreamIndex = 0;
    PlayerState *playerState;

protected:
    virtual void run();
};


#endif //XPLAY_IDEMUX_H
