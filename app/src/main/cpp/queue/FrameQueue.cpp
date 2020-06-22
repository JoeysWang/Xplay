//
// Created by cain on 2018/12/21.
//

#include "FrameQueue.h"

FrameQueue::FrameQueue(int max_size, int keep_last) : Queue<XData>(max_size) {

//    memset(queue, 0, sizeof(Frame) * FRAME_QUEUE_SIZE);
//    this->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
//    this->keep_last = (keep_last != 0);
//    for (int i = 0; i < this->max_size; ++i) {
//        queue[i].frame = av_frame_alloc();
//    }
//    abort_request = 1;
//    rindex = 0;
//    windex = 0;
//    size = 0;
//    show_index = 0;
}


void FrameQueue::start() {
//    mMutex.lock();
//    abort_request = 0;
//    mCondition.notify_all();
//    mMutex.unlock();
}

void FrameQueue::abort() {
    quit();
//    mMutex.lock();
//    abort_request = 1;
//    mCondition.notify_all();
//    mMutex.unlock();
}

XData *FrameQueue::currentFrame() {
    XData *data;
    peek(*data);
    return data;
//    return &queue[(rindex + show_index) % max_size];
}

XData *FrameQueue::nextFrame() {
    return &queue[(rindex + show_index + 1) % max_size];
}

XData *FrameQueue::lastFrame() {
    return &queue[rindex];
}

XData *FrameQueue::peekWritable() {
    std::unique_lock<std::mutex> lock(mMutex);
    lock.lock();

    while (size >= max_size && !abort_request) {
        mCondition.wait(lock);
    }
    lock.unlock();

    if (abort_request) {
        return NULL;
    }

    return &queue[windex];
}

void FrameQueue::pushFrame() {
    if (++windex == max_size) {
        windex = 0;
    }
    mMutex.lock();
    size++;
    mCondition.notify_all();
    mMutex.unlock();
}

void FrameQueue::popFrame() {
    if (keep_last && !show_index) {
        show_index = 1;
        return;
    }
    unrefFrame(&queue[rindex]);
    if (++rindex == max_size) {
        rindex = 0;
    }
    mMutex.lock();
    size--;
    mCondition.notify_all();
    mMutex.unlock();
}

void FrameQueue::flush() {
    while (getFrameSize() > 0) {
        popFrame();
    }
}

int FrameQueue::getFrameSize() {
    return size - show_index;
}

void FrameQueue::unrefFrame(Frame *vp) {
    av_frame_unref(vp->frame);
    avsubtitle_free(&vp->sub);
}

int FrameQueue::getShowIndex() const {
    return show_index;
}
