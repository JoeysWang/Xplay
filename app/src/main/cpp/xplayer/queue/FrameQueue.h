

#ifndef MEDIAPLAYER_FRAMEQUEUE_H
#define MEDIAPLAYER_FRAMEQUEUE_H

#include <mutex>
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

class FrameQueue {

public:
    FrameQueue(int max_size, int keep_last);

    virtual ~FrameQueue();

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

public:
    std::mutex mMutex;
    std::condition_variable mNotFull;
    int abort_request;
    XData queue[FRAME_QUEUE_SIZE];
    int rindex;
    int windex;
    int size;
    int max_size;
    int keep_last;
    int show_index;
};


#endif //MEDIAPLAYER_FRAMEQUEUE_H
