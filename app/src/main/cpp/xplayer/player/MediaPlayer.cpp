//
// Created by ky611 on 2020/6/20.
//

#include "MediaPlayer.h"
#include "../demux/IDemux.h"
#include "../decode/IDecode.h"
#include "../resample/IResample.h"
#include "../audio/IAudioPlay.h"
#include "../video/IVideoView.h"
#include "../../XLog.h"

MediaPlayer::MediaPlayer() {
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

MediaPlayer *MediaPlayer::get(unsigned char index) {
    return new MediaPlayer();
}

void MediaPlayer::setDataSource(std::string &url) {
    playerState->url = url;
    const char *path = playerState->url.c_str();
    std::unique_lock<std::mutex> lock(mutex);
    LOGD("IPlayer::setDataSource %s", url.c_str());
    if (!demux || !demux->open(path)) {
        LOGE("IPlayer::open demux error ");
        return;
    }
    demux->playerHandler = getHandler();
    LOGE("IPlayer::open demux success ");
    if (!videoDecode ||
        !videoDecode->openDecode(demux->getVideoParameter(),
                                 demux->getVideoStream(),
                                 demux->formatContext)) {
        LOGE("IPlayer videoDecode->open error ");
    }
    videoDecode->playerHandler = getHandler();
    if (!audioDecode ||
        !audioDecode->openDecode(demux->getAudioParameter(),
                                 demux->getAudioStream(),
                                 demux->formatContext)) {
        LOGE("IPlayer audioDecode->open error ");
    }
    audioDecode->playerHandler = getHandler();
    LOGD("IPlayer::setDataSource success  ");
}

void MediaPlayer::openSource() {
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

void MediaPlayer::start() {
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


bool MediaPlayer::initView(void *window) {
    mutex.lock();
    this->window = window;
    if (videoView) {
        videoView->setRender(window);
    }
    mutex.unlock();
    return true;
}

void MediaPlayer::pause() {
    mutex.lock();
    playerState->pauseRequest = 1;
    LOGD("IPlayer::pause %p = %d", playerState, playerState->pauseRequest);
    mutex.unlock();
}

void MediaPlayer::resume() {
    mutex.lock();
    LOGD("IPlayer::resume");
    playerState->pauseRequest = 0;
    mutex.unlock();
}

void MediaPlayer::release() {
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
    window = nullptr;
    delete jniListener;
    jniListener = nullptr;

    LOGD("IPlayer::release success");
}

void MediaPlayer::stop() {
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

int MediaPlayer::getVideoWidth() {
    std::unique_lock<std::mutex> lock(mutex);
    if (videoDecode) {
        return videoDecode->codecContext->width;
    }
    return 0;
}

int MediaPlayer::getVideoHeight() {
    std::unique_lock<std::mutex> lock(mutex);
    if (videoDecode) {
        return videoDecode->codecContext->height;
    }
    return 0;
}

void MediaPlayer::setListener(MediaPlayerListener *listener) {
    if (jniListener != nullptr) {
        delete jniListener;
        jniListener = nullptr;
    }
    MediaPlayer::jniListener = listener;
}

void MediaPlayer::handleMessage(XMessage *message) {
    LOGI("MediaPlayer::handleMessage what=%d", message->what);
}
