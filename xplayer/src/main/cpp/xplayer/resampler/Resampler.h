//
// Created by 王越 on 2020/8/9.
//

#ifndef XPLAY_RESAMPLER_H
#define XPLAY_RESAMPLER_H

#include <memory>
#include "../data/PlayerState.h"
#include "../data/FrameData.h"
#include "../audio/IAudioPlay.h"
#include "../audio/SLAudioPlayer.h"

class Resampler {

public:
    Resampler(const std::shared_ptr<PlayerState> &playerState);

    virtual bool open(DecodeParam in, DecodeParam out);

    virtual FrameData resample(FrameData *in);

    void setCallback(AudioPCMCallback pcmCallback, void *context) {
        audioPlayer->setCallback(pcmCallback, context);
    };

    void update(FrameData *data);

    void quit();

private:
    std::shared_ptr<PlayerState> playerState;
    std::unique_ptr<SLAudioPlayer> audioPlayer;
    SwrContext *swrContext;
    int outChannels = 2;
    int outFormat = 1;//AV_SAMPLE_FMT_S16

};


#endif //XPLAY_RESAMPLER_H
