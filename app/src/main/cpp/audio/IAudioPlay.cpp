//
// Created by ky611 on 2020/6/16.
//

#include "IAudioPlay.h"
#include "../XLog.h"
IAudioPlay::IAudioPlay(PlayerState *playerState) : playerState(playerState) {

}

XData IAudioPlay::getData() {
    XData d;
    std::unique_lock<std::mutex> lock(framesMutex);
    if (frames.empty()) {
        notEmpty.wait(lock);
    }
    if (isExit){
        return d;
    }
    d = frames.front();
    pts = d.pts;
    frames.pop_front();
    notFull.notify_all();
    return d;
}

void IAudioPlay::update(XData data) {
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

void IAudioPlay::setCallback(AudioPCMCallback pcmCallback, void *context) {
    IAudioPlay::callback = pcmCallback;
    IAudioPlay::callbackContext = context;
}

IAudioPlay::~IAudioPlay() {
    mutex.lock();
    frames.clear();
    callback = NULL;
    callbackContext = nullptr;
    mutex.unlock();
}

