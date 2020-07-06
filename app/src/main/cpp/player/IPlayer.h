//
// Created by ky611 on 2020/6/20.
//

#ifndef XPLAY_IPLAYER_H
#define XPLAY_IPLAYER_H

#include <mutex>
#include "../XThread.h"
#include "../data/XParameter.h"
#include "../sync/MediaSync2.h"
#include "../demux/IDemux.h"
#include "../decode/IDecode.h"
#include "../video/IVideoView.h"
#include "../audio/IAudioPlay.h"
#include "../resample/IResample.h"

class IPlayer : public XThread {

public:
    static IPlayer *get(unsigned char index = 0);

    virtual bool open(const char *path);

    virtual bool initView(void *window);

    virtual void pause();

    virtual void resume();

    virtual void stop();

    virtual void release();

    PlayerState *playerState;
    MediaSync2 *mediaSync;
    IDemux *demux = 0;
    IDecode *audioDecode = 0;
    IDecode *videoDecode = 0;
    IResample *resample = 0;
    IVideoView *videoView = 0;
    IAudioPlay *audioPlay = 0;

    //音频输出配置
    XParameter audioOutParam;

    IPlayer();

    void start() override;

    void run() override;


public:
    void *window;
    std::mutex mutex;
};


#endif //XPLAY_IPLAYER_H
