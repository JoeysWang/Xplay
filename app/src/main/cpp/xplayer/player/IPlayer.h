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

class IPlayer {

public:
    IPlayer();

    void openSource();

    void start();


    static IPlayer *get(unsigned char index = 0);

    virtual bool initView(void *window);

    virtual void pause();

    virtual void resume();

    virtual void stop();

    virtual void release();

    void setDataSource(std::string &url);

public:
    PlayerState *playerState;
    MediaSync2 *mediaSync = nullptr;
    IDemux *demux = 0;
    IDecode *audioDecode = 0;
    IDecode *videoDecode = 0;
    IResample *resample = 0;
    IVideoView *videoView = 0;
    IAudioPlay *audioPlay = 0;
    //音频输出配置
    XParameter audioOutParam;

public:
    void *window;
    std::mutex mutex;

    int getVideoWidth();

    int getVideoHeight();
};


#endif //XPLAY_IPLAYER_H
