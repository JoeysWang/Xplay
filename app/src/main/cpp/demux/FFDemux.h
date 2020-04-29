//
// Created by 王越 on 2020/4/26.
//

#ifndef XPLAY_FFDEMUX_H
#define XPLAY_FFDEMUX_H


struct AVFormatContext;

#include "../data/XData.h"
#include "IDemux.h"

class FFDemux : public IDemux {
public:
    bool open(const char *url) override;

    XData read() override;

    FFDemux();

    ~FFDemux();

    XParameter getVideoParameter() override;

    XParameter getAudioParameter() override;

private:
    AVFormatContext *ic = 0;
    int audioStreamIndex =0;
    int videoStreamIndex =0;

};


#endif //XPLAY_FFDEMUX_H
