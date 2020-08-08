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

class FFPlayer : HandlerThread {

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

private:
    void init();

    void handleMessage(XMessage *message) override;

    void onHandlerCreate() override;

protected:
    std::shared_ptr<PlayerState> playerState;
    std::shared_ptr<VideoDecode> videoDecode;
    std::shared_ptr<AudioDecode> audioDecode;

    std::unique_ptr<MediaSync> mediaSync;
    std::unique_ptr<Demuxer> demuxer;


};


#endif //FFPLAYER_FFPLAYER_H
