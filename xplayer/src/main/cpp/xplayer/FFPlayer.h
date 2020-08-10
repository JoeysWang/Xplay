//
// Created by 王越 on 2020/8/6.
//

#ifndef FFPLAYER_FFPLAYER_H
#define FFPLAYER_FFPLAYER_H


#include "data/PlayerState.h"
#include "player/MediaSync.h"
#include "demux/Demuxer.h"
#include "../xhandler/HandlerThread.h"
#include "decode/VideoDecode.h"
#include "decode/AudioDecode.h"
#include "resampler/Resampler.h"
#include "video/IVideoView.h"
#include "utils/MediaPlayerListener.h"
#include <memory>

class FFPlayer : public HandlerThread {

public:
    FFPlayer();

    void setDataSource(const char *url);

    void setNativeWindow(void *win);

    void playOrPause();

    void release();

    int getVideoWidth();

    int getVideoHeight();

    int getDuration();

    bool isPlaying();

    void setJavaHandler(const std::shared_ptr<MediaPlayerListener> &javaHandler);

private:
    void init();

    void onLooperPrepared() override;

public:
    void handleMessage(XMessage *message) override;

    virtual ~FFPlayer();

protected:
    std::shared_ptr<PlayerState> playerState;
    std::shared_ptr<VideoDecode> videoDecode;
    std::shared_ptr<AudioDecode> audioDecode;
    std::shared_ptr<Resampler> resampler;
    std::shared_ptr<IVideoView> videoView;
    std::shared_ptr<MediaPlayerListener> javaHandler;
    std::shared_ptr<XHandler> handler;

    std::unique_ptr<MediaSync> mediaSync;
    std::unique_ptr<Demuxer> demuxer;


    std::mutex mutex;
    const char *url = nullptr;
    void *win;

};


#endif //FFPLAYER_FFPLAYER_H
