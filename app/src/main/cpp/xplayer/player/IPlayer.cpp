//
// Created by ky611 on 2020/6/20.
//

#include "IPlayer.h"
#include "../demux/IDemux.h"
#include "../decode/IDecode.h"
#include "../resample/IResample.h"
#include "../audio/IAudioPlay.h"
#include "../video/IVideoView.h"
#include "../../XLog.h"

IPlayer::IPlayer() {
    mediaSync = nullptr;
    demux = nullptr;
    audioDecode = nullptr;
    videoDecode = nullptr;
    resample = nullptr;
    videoView = nullptr;
    audioPlay = nullptr;
    playerState = new PlayerState();
    LOGI("IPlayer constructor playerState=%p", playerState);
}

IPlayer *IPlayer::get(unsigned char index) {
    return new IPlayer();
}

void IPlayer::setDataSource(std::string &url) {
    playerState->url = url;
    const char *path = playerState->url.c_str();
    std::unique_lock<std::mutex> lock(mutex);
    LOGD("IPlayer::setDataSource %s", url.c_str());
    if (!demux || !demux->open(path)) {
        LOGE("IPlayer::open demux error ");
        return;
    }
    LOGE("IPlayer::open demux success ");
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
    LOGD("IPlayer::setDataSource success  ");
}

void IPlayer::openSource() {
    std::unique_lock<std::mutex> lock(mutex);
    if (playerState->url.empty()) {
        LOGE("IPlayer::open url is empty ");
        return;
    }
    const char *path = playerState->url.c_str();
    if (!demux || !demux->open(path)) {
        LOGE("IPlayer::open demux error ");
        return;
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
    LOGI("IPlayer::openSource success!");
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
    LOGD("IPlayer::pause %p = %d", playerState, playerState->pauseRequest);
    mutex.unlock();
}

void IPlayer::resume() {
    mutex.lock();
    LOGD("IPlayer::resume");
    playerState->pauseRequest = 0;
    mutex.unlock();
}

void IPlayer::release() {
    LOGE("IPlayer::release ========");
    delete mediaSync;
    mediaSync = nullptr;
    delete demux;
    demux = nullptr;
    delete audioDecode;
    audioDecode = nullptr;
    delete videoDecode;
    videoDecode = nullptr;
    delete resample;
    resample = nullptr;
    delete videoView;
    videoView = nullptr;
    delete audioPlay;
    audioPlay = nullptr;
    delete playerState;
    playerState = nullptr;
    LOGD("IPlayer::release success");
}

void IPlayer::stop() {
    mutex.lock();
    LOGE("IPlayer::stop ========");
    playerState->abortRequest = 1;
    if (mediaSync) {
        LOGI("mediaSync->stop() ");
        mediaSync->stop();
    }
    if (audioPlay) {
        LOGI("audioPlay->stop() ");
        audioPlay->stop();
    }
    if (demux) {
        LOGI("demux->stop() ");
        demux->stop();
    }
    if (audioDecode) {
        LOGI("audioDecode->stop() ");
        audioDecode->stop();
    }
    if (videoDecode) {
        LOGI("videoDecode->stop() ");
        videoDecode->stop();
    }
    if (resample) {
        LOGI("resample->stop() ");
        resample->stop();
    }

    mutex.unlock();
}

int IPlayer::getVideoWidth() {
    std::unique_lock<std::mutex> lock(mutex);
    if (videoDecode) {
        return videoDecode->codecContext->width;
    }
    return 0;
}

int IPlayer::getVideoHeight() {
    std::unique_lock<std::mutex> lock(mutex);
    if (videoDecode) {
        return videoDecode->codecContext->height;
    }
    return 0;
}
