//
// Created by 王越 on 2020/8/6.
//

#ifndef FFPLAYER_DEMUXER_H
#define FFPLAYER_DEMUXER_H

#include "../data/PlayerState.h"
#include "../data/DecodeParam.h"
#include "../decode/VideoDecode.h"
#include "../decode/AudioDecode.h"

extern "C" {
#include "libavformat/avformat.h"
};

class Demuxer {
public:

    Demuxer(std::shared_ptr<PlayerState> const &state);

    void getAllStream();

    void openSource(const char *url);

    AVStream *getAudioStream();

    AVStream *getVideoStream();

    virtual ~Demuxer();

    DecodeParam getVideoParameter();

    DecodeParam getAudioParameter();

    void quit();

    void setPlayerHandler(const std::shared_ptr<XHandler> &playerHandler);

private:
    void readPacket();

private:
    std::shared_ptr<PlayerState> playerState;
    bool isRunning=true;
public:
    AVFormatContext *formatContext = 0;
    std::shared_ptr<VideoDecode> videoDecode;
    std::shared_ptr<AudioDecode> audioDecode;
    std::shared_ptr<XHandler> playerHandler;

protected:
    std::mutex mutex;
    int videoStreamIndex;
    int audioStreamIndex;
};


#endif //FFPLAYER_DEMUXER_H
