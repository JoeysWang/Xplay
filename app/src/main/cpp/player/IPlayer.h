//
// Created by ky611 on 2020/6/20.
//

#ifndef XPLAY_IPLAYER_H
#define XPLAY_IPLAYER_H

#include <mutex>
#include "../XThread.h"
#include "../data/XParameter.h"

class IDemux;

class IDecode;

class IResample;

class IVideoView;

class IAudioPlay;

class IPlayer : public XThread {

public:
    static IPlayer *get(unsigned char index = 0);

    virtual bool open(const char *path);

    virtual bool initView(void *window);


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

    void Main() override;

public:
    void *window;
    std::mutex mutex;
};


#endif //XPLAY_IPLAYER_H
