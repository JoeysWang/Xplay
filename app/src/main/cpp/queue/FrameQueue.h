//
// Created by cain on 2018/12/21.
//

#ifndef MEDIAPLAYER_FRAMEQUEUE_H
#define MEDIAPLAYER_FRAMEQUEUE_H

#include <mutex>
#include "Queue.h"
#include <condition_variable>
#include "../data/XData.h"

extern "C" {
#include <libavcodec/avcodec.h>
};

#define FRAME_QUEUE_SIZE 10

//typedef struct Frame {
//    AVFrame *frame;
//    AVSubtitle sub;
//    double pts;           /* presentation timestamp for the frame */
//    double duration;      /* estimated duration of the frame */
//    int width;
//    int height;
//    int format;
//    int uploaded;
//} Frame;

class FrameQueue :public Queue<XData>{

public:
    FrameQueue(int max_size, int keep_last);

    void start();

    void abort();

    XData *currentFrame();

    XData *nextFrame();

    XData *lastFrame();

    XData *peekWritable();

    void pushFrame();

    void popFrame();

    void flush();

    int getFrameSize();

    int getShowIndex() const;

private:
    void unrefFrame(XData *vp);

//private:
//    std::mutex mMutex;
//    std::condition_variable mCondition;
//    int abort_request;
//    Frame queue[FRAME_QUEUE_SIZE];
//    int rindex;
//    int windex;
//    int size;
//    int max_size;
//    int keep_last;
//    int show_index;
};


#endif //MEDIAPLAYER_FRAMEQUEUE_H
