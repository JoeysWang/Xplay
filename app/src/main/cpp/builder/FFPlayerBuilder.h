//
// Created by ky611 on 2020/6/20.
//

#ifndef XPLAY_FFPLAYERBUILDER_H
#define XPLAY_FFPLAYERBUILDER_H


#include "IPlayerBuilder.h"

class FFPlayerBuilder : public IPlayerBuilder {
public:
    static FFPlayerBuilder *get() {
        static FFPlayerBuilder builder;
        return &builder;
    }

protected:
    FFPlayerBuilder() {};

    IDecode *createAudioDecode(PlayerState *playerState) override;

    IResample *createResample(PlayerState *playerState) override;

    IVideoView *createVideoView() override;

    IAudioPlay *createAudioPlay(PlayerState *playerState) override;

    IPlayer *createPlayer(unsigned char index = 0) override;

    IDecode *createVideoDecode(PlayerState *playerState) override;

    IDemux *createDemux(PlayerState *playerState) override;

};


#endif //XPLAY_FFPLAYERBUILDER_H
