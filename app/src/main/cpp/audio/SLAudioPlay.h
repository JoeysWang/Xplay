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

    SLuint32 getCurSampleRate(int sample_rate);
    SLObjectItf engineSL = NULL;
    SLEngineItf engineItf = NULL;
    SLObjectItf mix = NULL;
    SLObjectItf player = NULL;
    SLPlayItf iplayer = NULL;
    SLAndroidSimpleBufferQueueItf pcmQueue = NULL;
};


#endif //XPLAY_SLAUDIOPLAY_H
