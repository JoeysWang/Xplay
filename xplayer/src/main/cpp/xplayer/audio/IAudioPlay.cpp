//
// Created by 王越 on 2020/8/9.
//

#include <XLog.h>
#include "IAudioPlay.h"

IAudioPlay::IAudioPlay(const std::shared_ptr<PlayerState> &playerState) : playerState(
        playerState) {
    frames = std::make_unique<Queue<FrameData>>(10);
    frames->tag = "IAudioPlay";
}

IAudioPlay::~IAudioPlay() {
    LOGI("IAudioPlay::~IAudioPlay");
    isExit = true;
    frames->clear();
    callback = NULL;
    callbackContext = nullptr;
}

void IAudioPlay::update(FrameData data) {
    //压入缓冲队列
    if (isExit) { return; }
    frames->push(data);
}

FrameData IAudioPlay::getData() {
    FrameData d;
    if (isExit) { return d; }
    frames->pop(d);
    return d;
}

void IAudioPlay::setCallback(AudioPCMCallback pcmCallback, void *context) {
    IAudioPlay::callback = pcmCallback;
    IAudioPlay::callbackContext = context;
}

void IAudioPlay::stop() {
    LOGD("IAudioPlay::stop");
}

