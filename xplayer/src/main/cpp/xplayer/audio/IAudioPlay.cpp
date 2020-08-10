//
// Created by 王越 on 2020/8/9.
//

#include <XLog.h>
#include "IAudioPlay.h"

IAudioPlay::IAudioPlay(const std::shared_ptr<PlayerState> &playerState) : playerState(
        playerState) {
}

IAudioPlay::~IAudioPlay() {
    LOGI("IAudioPlay::~IAudioPlay");
    isExit = true;
    notEmpty.notify_all();
    notFull.notify_all();
    frames.clear();
    callback = NULL;
    callbackContext = nullptr;
}

void IAudioPlay::update(FrameData data) {
    //压入缓冲队列
    std::unique_lock<std::mutex> lock(framesMutex);
    if (data.size <= 0)
        return;

    if (frames.size() > maxFrameBuffer) {
        notFull.wait(lock);
    }
    frames.push_back(data);
    notEmpty.notify_all();
}

FrameData IAudioPlay::getData() {
    FrameData d;
    std::unique_lock<std::mutex> lock(framesMutex);
    if (frames.empty()) {
        notEmpty.wait(lock);
    }
    if (isExit) {
        return d;
    }
    d = frames.front();
    pts = d.pts;
    frames.pop_front();
    notFull.notify_all();
    return d;
}

void IAudioPlay::setCallback(AudioPCMCallback pcmCallback, void *context) {
    IAudioPlay::callback = pcmCallback;
    IAudioPlay::callbackContext = context;
}

void IAudioPlay::stop() {
    LOGD("IAudioPlay::stop");
}

