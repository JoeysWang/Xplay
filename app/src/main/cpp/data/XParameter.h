//
// Created by 王越 on 2020/4/29.
//

#ifndef XPLAY_XPARAMETER_H
#define XPLAY_XPARAMETER_H


extern "C" {

#include <libavcodec/avcodec.h>
};

class XParameter {
public:
    AVCodecParameters *parameters = 0;

    int channels = 2;
    int sampleRate = 44100;

};


#endif //XPLAY_XPARAMETER_H
