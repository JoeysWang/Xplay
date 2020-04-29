//
// Created by 王越 on 2020/4/26.
//

#ifndef XPLAY_FFDEMUX_H
#define XPLAY_FFDEMUX_H


struct AVFormatContext;

#include "XData.h"
#include "IDemux.h"

class FFDemux : public IDemux {
public:
    bool open(const char *url) override;

    XData read() override;

    FFDemux();

    ~FFDemux();

private:
    AVFormatContext *ic = 0;
};


#endif //XPLAY_FFDEMUX_H
