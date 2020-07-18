//
// Created by ky611 on 2020/6/20.
//

#ifndef XPLAY_MEDIAPLAYER_H
#define XPLAY_MEDIAPLAYER_H

#include <mutex>
#include "../XThread.h"
#include "../data/XParameter.h"
#include "../sync/MediaSync2.h"
#include "../demux/IDemux.h"
#include "../decode/IDecode.h"
#include "../video/IVideoView.h"
#include "../audio/IAudioPlay.h"
#include "../resample/IResample.h"

class MediaPlayerListener {
public:
    virtual void notify(int msg, int ext1, int ext2, void *obj) {}
};

class MediaPlayer :HandlerThread{

public:
    MediaPlayer();

    void openSource();

    void start();


    static MediaPlayer *get(unsigned char index = 0);

    virtual bool initView(void *window);

    virtual void pause();

    virtual void resume();

    virtual void stop();

    virtual void release();

    void setDataSource(std::string &url);

    void setListener(MediaPlayerListener *mListener);

private:
    void handleMessage(XMessage *message) override;

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
    MediaPlayerListener *jniListener = nullptr;;
public:
    void *window;
    std::mutex mutex;

    int getVideoWidth();

    int getVideoHeight();
};


#endif //XPLAY_MEDIAPLAYER_H
