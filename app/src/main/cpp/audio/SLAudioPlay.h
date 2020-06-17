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
    bool startPlay(XParameter out) override;
    void pcmCallback(SLAndroidSimpleBufferQueueItf bf, void *context);
    SLEngineItf createSL();


public:
};


#endif //XPLAY_SLAUDIOPLAY_H
