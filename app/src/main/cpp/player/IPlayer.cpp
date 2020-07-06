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

IPlayer::IPlayer() {
    playerState = new PlayerState();
    LOGI("IPlayer constructor playerState=%p",playerState);
}
static IPlayer players[1];

IPlayer *IPlayer::get(unsigned char index) {
    return &players[index];
}

void IPlayer::run() {

}

bool IPlayer::open(const char *path) {
    mutex.lock();
    if (!demux || !demux->open(path)) {
        LOGE("IPlayer::open demux error ");
        return false;
    }
    if (!videoDecode ||
        !videoDecode->openDecode(demux->getVideoParameter(),
                                 demux->getVideoStream(),
                                 demux->formatContext)) {
        LOGE("IPlayer videoDecode->open error ");
    }

    if (!audioDecode ||
        !audioDecode->openDecode(demux->getAudioParameter(),
                                 demux->getAudioStream(),
                                 demux->formatContext)) {
        LOGE("IPlayer audioDecode->open error ");
    }
    audioOutParam = (demux->getAudioParameter());

    if (!resample || !resample->open(demux->getAudioParameter(), audioOutParam)) {
        LOGE("IPlayer  resample->open error ");
    }
    if (window) {
        videoView->setRender(window);
    }
    mutex.unlock();
    LOGI("IPlayer::open success!");
    return true;
}

void IPlayer::start() {
    mutex.lock();
    if (!demux) {
        LOGE("IPlayer startPlay error ");
        mutex.unlock();
        return;
    }
    audioDecode->start();
    if (audioOutParam.sampleRate <= 0) {
        audioOutParam = demux->getAudioParameter();
    }
    audioPlay->startPlay(audioOutParam);
    resample->addObserver(audioPlay);

    mediaSync = new MediaSync2(playerState, audioDecode, videoDecode);
    mediaSync->setAudioPlay(audioPlay);
    mediaSync->setResample(resample);
    mediaSync->setVideoView(videoView);

    videoDecode->start();
    resample->start();
    demux->start();
    mediaSync->start();

    XThread::start();
    mutex.unlock();
}


bool IPlayer::initView(void *window) {
    mutex.lock();
    this->window = window;
    if (videoView) {
        videoView->setRender(window);
    }
    mutex.unlock();
    return true;
}

void IPlayer::pause() {
    mutex.lock();
    playerState->pauseRequest = 1;
    LOGD("IPlayer::pause %p = %d",playerState,playerState->pauseRequest);
    mutex.unlock();
}

void IPlayer::resume() {
    mutex.lock();
    LOGD("IPlayer::resume");
    playerState->pauseRequest = 0;
    mutex.unlock();
}

void IPlayer::release() {
    mutex.lock();
    LOGD("IPlayer::release");
    delete demux;
    delete audioDecode;
    delete videoDecode;
    delete resample;
    delete videoView;
    delete audioPlay;
    mutex.unlock();
}

void IPlayer::stop() {
    mutex.lock();
    XThread::stop();
    playerState->abortRequest = 1;
    if (audioDecode) { audioDecode->stop(); }
    if (videoDecode) { videoDecode->stop(); }
    if (resample) { resample->stop(); }
    mutex.unlock();
}
