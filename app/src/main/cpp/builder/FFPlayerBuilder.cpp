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

IDemux *FFPlayerBuilder::createDemux() {
    return new FFDemux();
}

IDecode *FFPlayerBuilder::createAudioDecode() {
    return new AudioDecode();
}

IDecode *FFPlayerBuilder::createVideoDecode() {
    return new VideoDecode();

}

IResample *FFPlayerBuilder::createResample() {
    return new FFResample();
}

IVideoView *FFPlayerBuilder::createVideoView() {
    return new GLVideoView();
}

IAudioPlay *FFPlayerBuilder::createAudioPlay() {
    return new SLAudioPlay();
}

IPlayer *FFPlayerBuilder::createPlayer(unsigned char index) {
    return IPlayer::get(index);
}

