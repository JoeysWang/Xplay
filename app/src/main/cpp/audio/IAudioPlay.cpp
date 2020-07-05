//
// Created by ky611 on 2020/6/16.
//

#include "IAudioPlay.h"
#include "../XLog.h"

XData IAudioPlay::getData() {
    XData d;
    std::unique_lock<std::mutex> lock(framesMutex);
    if (frames.empty()) {
//        LOGE("IAudioPlay empty wait");
        notEmpty.wait(lock);
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
//        LOGE("IAudioPlay full wait");
        notFull.wait(lock);
    }
    frames.push_back(data);
    notEmpty.notify_all();

}

void IAudioPlay::setCallback(AudioPCMCallback  pcmCallback, void *context) {
    IAudioPlay::callback = pcmCallback;
    IAudioPlay::callbackContext = context;
}
