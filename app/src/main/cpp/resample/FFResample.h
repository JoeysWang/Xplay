//
// Created by ky611 on 2020/6/14.
//

#ifndef XPLAY_FFRESAMPLE_H
#define XPLAY_FFRESAMPLE_H


#include "IResample.h"

extern "C" {
#include <libswresample/swresample.h>
};

class FFResample : public IResample {
public:
    FFResample(PlayerState *playerState);

    virtual bool open(XParameter in, XParameter out) override ;

    XData resample(XData in) override;

    virtual ~FFResample();

public:
    SwrContext *swrContext;

};


#endif //XPLAY_FFRESAMPLE_H
