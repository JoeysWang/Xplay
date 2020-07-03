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

    IDemux *createDemux() override;

    IDecode *createAudioDecode() override;

    IResample *createResample() override;

    IVideoView *createVideoView() override;

    IAudioPlay *createAudioPlay() override;

    IPlayer *createPlayer(unsigned char index = 0) override;

    IDecode *createVideoDecode() override;

};


#endif //XPLAY_FFPLAYERBUILDER_H
