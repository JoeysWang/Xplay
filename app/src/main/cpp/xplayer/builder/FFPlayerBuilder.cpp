//
// Created by ky611 on 2020/6/20.
//

#include "FFPlayerBuilder.h"
#include "../demux/FFDemux.h"
#include "../decode/VideoDecode.h"
#include "../resample/FFResample.h"
#include "../video/GLVideoView.h"
#include "../audio/SLAudioPlay.h"
#include "../decode/AudioDecode.h"


IDemux *FFPlayerBuilder::createDemux(PlayerState *playerState) {
    return new FFDemux(playerState);
}

IDecode *FFPlayerBuilder::createAudioDecode(PlayerState *playerState) {
    return new AudioDecode(playerState);
}

IDecode *FFPlayerBuilder::createVideoDecode(PlayerState *playerState) {
    return new VideoDecode(playerState);

}

IResample *FFPlayerBuilder::createResample(PlayerState *playerState) {
    return new FFResample(playerState);
}

IVideoView *FFPlayerBuilder::createVideoView() {
    return new GLVideoView();
}

IAudioPlay *FFPlayerBuilder::createAudioPlay(PlayerState *playerState) {
    return new SLAudioPlay(playerState);
}

MediaPlayer *FFPlayerBuilder::createPlayer(unsigned char index) {
    return MediaPlayer::get(index);
}

