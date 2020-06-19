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
            notEmpty.wait(lock);
        }
        d = frames.front();
        frames.pop_front();
        notFull.notify_all();
    }


    return d;
}

void IAudioPlay::update(XData data) {

    //压入缓冲队列
    LOGD("IAudioPlay::update %d ", frames.size());

    if (!data.data || data.size <= 0)
        return;
    while (!isExit) {
        std::unique_lock<std::mutex> lock(framesMutex);

        if (frames.size() > maxFrameBuffer) {
            notFull.wait(lock);
        }
        frames.push_back(data);
        notEmpty.notify_all();
        break;
    }

}
