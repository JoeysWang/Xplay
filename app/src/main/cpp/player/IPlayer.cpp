//
// Created by ky611 on 2020/6/20.
//

#include "IPlayer.h"
#include "../demux/IDemux.h"
#include "../decode/IDecode.h"
#include "../resample/IResample.h"
#include "../audio/IAudioPlay.h"
#include "../video/IVideoView.h"
#include "../XLog.h"

IPlayer *IPlayer::get(unsigned char index) {
    static IPlayer players[256];
    return &players[index];
}

bool IPlayer::open(const char *path) {

    if (!demux || !demux->open(path)) {
        LOGE("IPlayer::open demux error ");
        return false;
    }
    if (!videoDecode || !videoDecode->open(demux->getVideoParameter())) {
        LOGE("IPlayer videoDecode->open error ");
    }

    if (!audioDecode || !audioDecode->open(demux->getAudioParameter())) {
        LOGE("IPlayer audioDecode->open error ");
    }
    audioOutParam = (demux->getAudioParameter());

    if (!resample || !resample->open(demux->getAudioParameter(), audioOutParam)) {
        LOGE("IPlayer  resample->open error ");
    }
    if (window) {
        videoView->setRender(window);
    }
    LOGI("IPlayer::open success!");
    return true;
}

void IPlayer::start() {
    if (!demux) {
        LOGE("IPlayer startPlay error ");
        return;
    }

    audioDecode->start();

    if (audioOutParam.sampleRate <= 0) {
        audioOutParam = demux->getAudioParameter();
    }
    audioPlay->startPlay(audioOutParam);

    videoDecode->addObserver(videoView);
    videoDecode->start();

    resample->addObserver(audioPlay);
    resample->start();

    demux->start();

}


bool IPlayer::initView(void *window) {
    this->window = window;
    if (videoView) {
        videoView->setRender(window);
    }
    return true;
}

IPlayer::IPlayer() {

}

