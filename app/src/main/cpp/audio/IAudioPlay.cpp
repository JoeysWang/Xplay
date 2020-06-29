//
// Created by ky611 on 2020/6/16.
//

#include "IAudioPlay.h"
#include "../XLog.h"

XData IAudioPlay::getData() {
    XData d;
    while (!isExit) {
        std::unique_lock<std::mutex> lock(framesMutex);
        if (frames.empty()) {
            LOGE("IAudioPlay empty wait");
            notEmpty.wait(lock);
        }
        d = frames.front();
        pts = d.pts;
        frames.pop_front();
        notFull.notify_all();
        break;
    }

    return d;
}

void IAudioPlay::update(XData data) {
    //压入缓冲队列
    if (!data.resampleData || data.size <= 0)
        return;
    while (!isExit) {
        std::unique_lock<std::mutex> lock(framesMutex);

        if (frames.size() > maxFrameBuffer) {
            LOGE("IAudioPlay full wait");
            notFull.wait(lock);
        }
        frames.push_back(data);
        notEmpty.notify_all();
        break;
    }

}