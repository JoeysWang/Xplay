

#ifndef MEDIAPLAYER_FRAMEQUEUE_H
#define MEDIAPLAYER_FRAMEQUEUE_H

#include <mutex>
#include <condition_variable>
#include "../data/FrameData.h"

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

    void quit();

    FrameData *currentFrame();

    FrameData *nextFrame();

    FrameData *lastFrame();

    FrameData *peekWritable();

    void pushFrame();

    void popFrame();

    void flush();

    int getFrameSize();

    int getShowIndex() const;

private:
    void unrefFrame(FrameData *vp);

public:
    std::mutex mMutex;
    std::condition_variable mNotFull;
    std::atomic_int abort_request = {0};
    FrameData queue[FRAME_QUEUE_SIZE];
    int rindex = 0;
    int windex = 0;
    int size = 0;
    int max_size;
    int keep_last;
    int show_index;
};


#endif //MEDIAPLAYER_FRAMEQUEUE_H
