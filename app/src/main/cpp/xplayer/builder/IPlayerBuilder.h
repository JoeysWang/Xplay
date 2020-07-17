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
#include "../player/MediaPlayer.h"

class IPlayerBuilder {
public:
    virtual MediaPlayer *buildPlayer();

protected:

    virtual IDemux *createDemux(PlayerState *playerState) = 0;

    virtual IDecode *createAudioDecode(PlayerState *playerState) = 0;

    virtual IDecode *createVideoDecode(PlayerState *playerState) = 0;

    virtual IResample *createResample(PlayerState *playerState) = 0;

    virtual IVideoView *createVideoView() = 0;

    virtual IAudioPlay *createAudioPlay(PlayerState *playerState) = 0;

    virtual MediaPlayer *createPlayer(unsigned char index = 0) = 0;


};


#endif //XPLAY_IPLAYERBUILDER_H
