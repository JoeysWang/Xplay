//
// Created by 王越 on 2020/8/6.
//

#include "FFPlayer.h"
#include "player/PlayerMessage.h"

FFPlayer::FFPlayer() {

}

void FFPlayer::onHandlerCreate() {
    init();
}

void FFPlayer::init() {
    playerState = std::make_shared<PlayerState>();
    mediaSync = std::make_unique<MediaSync>(playerState);
    demuxer = std::make_unique<Demuxer>(playerState);
}

void FFPlayer::handleMessage(XMessage *message) {
    switch (message->what) {
        case MSG_OPEN_INPUT: {
            auto url = (const char *) message->obj;
            LOGI("FFPlayer::MSG_OPEN_INPUT %s", url);
            demuxer->openSource(url);
            break;
        }
    }
}

void FFPlayer::setNativeWindow(void *win) {

}

void FFPlayer::setDataSource(const char *url) {
    getHandler()->postMessage(MSG_OPEN_INPUT, (void *) url);
}

void FFPlayer::playOrPause() {

}

void FFPlayer::release() {

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

