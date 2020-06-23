
#include "FrameQueue.h"
#include "../XLog.h"

FrameQueue::FrameQueue(int max_size, int keep_last) {
    memset(queue, 0, sizeof(XData) * FRAME_QUEUE_SIZE);
    this->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
    this->keep_last = (keep_last != 0);
    for (int i = 0; i < this->max_size; ++i) {
        queue[i].data = (unsigned char *) av_frame_alloc();
    }
    abort_request = 1;
    rindex = 0;
    windex = 0;
    size = 0;
    show_index = 0;
}

FrameQueue::~FrameQueue() {
    for (int i = 0; i < max_size; ++i) {
        XData *vp = &queue[i];
        unrefFrame(vp);
        av_frame_free((AVFrame **) (&vp->frameDatas));
    }
}

void FrameQueue::start() {
    mMutex.lock();
    abort_request = 0;
    mNotFull.notify_all();
    mMutex.unlock();
}

void FrameQueue::abort() {
    mMutex.lock();
    abort_request = 1;
    mNotFull.notify_all();
    mMutex.unlock();
}

XData *FrameQueue::currentFrame() {
    return &queue[(rindex + show_index) % max_size];
}

XData *FrameQueue::nextFrame() {
    return &queue[(rindex + show_index + 1) % max_size];
}

XData *FrameQueue::lastFrame() {
    return &queue[rindex];
}

XData *FrameQueue::peekWritable() {
    std::unique_lock<std::mutex> lock(mMutex);
    if (size >= max_size && !abort_request) {
        LOGE("frame queue 已满");
        mNotFull.wait(lock);
    }
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
    mNotFull.notify_all();
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
    mNotFull.notify_all();
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

void FrameQueue::unrefFrame(XData *vp) {
    av_frame_unref((AVFrame *) (&vp->frameDatas));
//    avsubtitle_free(&vp->sub); 字幕还不支持吧
}

int FrameQueue::getShowIndex() const {
    return show_index;
}
