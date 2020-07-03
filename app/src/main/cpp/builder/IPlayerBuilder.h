//
// Created by ky611 on 2020/6/20.
//

#ifndef XPLAY_IPLAYERBUILDER_H
#define XPLAY_IPLAYERBUILDER_H


#include "../demux/IDemux.h"
#include "../decode/IDecode.h"
#include "../resample/IResample.h"
#include "../video/IVideoView.h"
#include "../audio/IAudioPlay.h"
#include "../player/IPlayer.h"

class IPlayerBuilder {
public:
    virtual  IPlayer *buildPlayer();

protected:
    virtual IDemux *createDemux() = 0;

    virtual IDecode *createAudioDecode() = 0;
    virtual IDecode *createVideoDecode() = 0;

    virtual IResample *createResample() = 0;

    virtual IVideoView *createVideoView() = 0;

    virtual IAudioPlay *createAudioPlay() = 0;

    virtual IPlayer *createPlayer(unsigned char  index=0) = 0;


};


#endif //XPLAY_IPLAYERBUILDER_H
