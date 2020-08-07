//
// Created by 王越 on 2020/8/6.
//

#ifndef FFPLAYER_DECODEPARAM_H
#define FFPLAYER_DECODEPARAM_H


#include <libavcodec/avcodec.h>

class DecodeParam {
public:
    AVCodecParameters *parameters = 0;

    int channels = 2;
    int sampleRate = 44100;
};


#endif //FFPLAYER_DECODEPARAM_H
