//
// Created by 王越 on 2020/8/6.
//

#include "FFPlayer.h"
#include "player/PlayerMessage.h"
#include "video/GLVideoView.h"

FFPlayer::FFPlayer() {

}

void FFPlayer::onLooperPrepared() {
    init();
}

void FFPlayer::init() {
    std::unique_lock<std::mutex> lock(mutex);

    playerState = std::make_shared<PlayerState>();
    demuxer = std::make_unique<Demuxer>(playerState);
    videoDecode = std::make_shared<VideoDecode>(playerState);
    audioDecode = std::make_shared<AudioDecode>(playerState);
//    videoView = std::make_shared<GLVideoView>(playerState);
//    resampler = std::make_shared<Resampler>(playerState);


//    mediaSync = std::make_unique<MediaSync>(playerState, videoDecode, audioDecode);
//    mediaSync->setVideoView(videoView);
//    mediaSync->setResampler(resampler);


    demuxer->videoDecode = videoDecode;
    demuxer->audioDecode = audioDecode;

    handler = std::make_unique<XHandler>(getLooper());
    handler->setCallBack(this);
    LOGI("FFPlayer::init success");
    if (url) {
        handler->postMessage(MSG_OPEN_INPUT, (void *) url);
    }
}

void FFPlayer::handleMessage(XMessage *message) {
    switch (message->what) {
        case MSG_OPEN_INPUT: {
            auto url = (const char *) message->obj;
            LOGI("FFPlayer::MSG_OPEN_INPUT %s", url);
            demuxer->openSource(url);

            videoDecode->openDecode(demuxer->getVideoParameter(), demuxer->formatContext,
                                    demuxer->getVideoStream());
            audioDecode->openDecode(demuxer->getAudioParameter(), demuxer->formatContext,
                                    demuxer->getAudioStream());
//            if (!resampler->open(demuxer->getAudioParameter(), demuxer->getAudioParameter())) {
//                LOGE("IPlayer  resample->open error ");
//            }
//            mediaSync->start();
            break;
        }
        case MSG_REQUEST_PAUSE: {
            playerState->pauseRequest = 1;
            break;
        }
        case MSG_REQUEST_START: {
            playerState->pauseRequest = 0;
            break;
        }
        case MSG_SET_SURFACE_WINDOW: {
//            videoView->setRenderSurface(win);
            break;
        }

    }
}

void FFPlayer::setNativeWindow(void *win) {
    this->win = win;
    if (handler)
        handler->postMessage(MSG_SET_SURFACE_WINDOW, (void *) win);
}

void FFPlayer::setDataSource(const char *url) {
    this->url = url;
    if (handler)
        handler->postMessage(MSG_OPEN_INPUT, (void *) url);
}

void FFPlayer::playOrPause() {
    std::unique_lock<std::mutex> lock(mutex);
    if (isPlaying()) {
        handler->postMessage(MSG_REQUEST_PAUSE, 0);
    } else {
        handler->postMessage(MSG_REQUEST_START, 0);
    }
}

void FFPlayer::release() {
    LOGI("FFPlayer::release");
    std::unique_lock<std::mutex> lock(mutex);
    quit();
    playerState->abortRequest = 1;
    url = nullptr;
    demuxer->quit();
    audioDecode->quit();
    videoDecode->quit();
//    mediaSync->stop();

    delete this;
}

int FFPlayer::getVideoWidth() {
    return 0;
}

int FFPlayer::getVideoHeight() {
    return 0;
}

int FFPlayer::getDuration() {
    return 0;
}

bool FFPlayer::isPlaying() {
    return !playerState->abortRequest && !playerState->pauseRequest;
}

FFPlayer::~FFPlayer() {
    LOGI("FFPlayer::~FFPlayer");
}

