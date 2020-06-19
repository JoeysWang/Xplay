//
// Created by ky611 on 2020/6/16.
//

#ifndef XPLAY_SLAUDIOPLAY_H
#define XPLAY_SLAUDIOPLAY_H


#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "IAudioPlay.h"

class SLAudioPlay : public IAudioPlay {
public:
    SLAudioPlay();

    virtual ~SLAudioPlay();

    bool startPlay(XParameter out) override;

    SLEngineItf createSL();

    void playCall(void *bufferQueue);

protected:
    unsigned char *buffer = 0;


};


#endif //XPLAY_SLAUDIOPLAY_H
