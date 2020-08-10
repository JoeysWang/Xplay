//
// Created by 王越 on 2020/8/9.
//

#ifndef XPLAY_SLAUDIOPLAYER_H
#define XPLAY_SLAUDIOPLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "IAudioPlay.h"

class SLAudioPlayer : public IAudioPlay{

protected:
    unsigned char *buffer = 0;

public:
    SLAudioPlayer(const std::shared_ptr<PlayerState> &playerState);

    bool startPlay(DecodeParam out) override;

    SLEngineItf createSL();

    void playCall(void *bufferQueue);

    void stop() override;

    virtual ~SLAudioPlayer();

protected:
    SLuint32 getCurSampleRate(int sample_rate);
    SLObjectItf engineSL = NULL;
    SLEngineItf engineItf = NULL;
    SLObjectItf mix = NULL;
    SLObjectItf player = NULL;
    SLPlayItf iplayer = NULL;
    SLAndroidSimpleBufferQueueItf pcmQueue = NULL;

};


#endif //XPLAY_SLAUDIOPLAYER_H
