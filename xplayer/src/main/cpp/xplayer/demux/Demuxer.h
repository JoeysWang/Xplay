//
// Created by 王越 on 2020/8/6.
//

#ifndef FFPLAYER_DEMUXER_H
#define FFPLAYER_DEMUXER_H

#include "../data/PlayerState.h"
#include "../data/DecodeParam.h"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
class Demuxer {
public:

    Demuxer(std::shared_ptr<PlayerState> const &state);

    void getAllStream();

    void openSource(const char *url);

    AVStream *getAudioStream();

    AVStream *getVideoStream();

    virtual ~Demuxer();

private:
    void readPacket();

private:
    AVStream *pStream = 0;
    AVFormatContext *formatContext = 0;
    std::shared_ptr<PlayerState> playerState;

    DecodeParam getVideoParameter();
    DecodeParam getAudioParameter();
};


#endif //FFPLAYER_DEMUXER_H
