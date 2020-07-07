//
// Created by 王越 on 2020/4/29.
//

#ifndef XPLAY_VIDEODECODE_H
#define XPLAY_VIDEODECODE_H


#include "IDecode.h"
#include <mutex>

struct AVCodecContext;
struct AVFrame;

class VideoDecode : public IDecode {
public:

    VideoDecode(PlayerState *playerState);

    void start() override;

    void close();

    int decodePacket() override;

protected:
    std::mutex mutex;
};


#endif //XPLAY_VIDEODECODE_H
