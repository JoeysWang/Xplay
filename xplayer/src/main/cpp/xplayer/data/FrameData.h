//
// Created by 王越 on 2020/8/7.
//

#ifndef FFPLAYER_FRAMEDATA_H
#define FFPLAYER_FRAMEDATA_H
extern "C" {
#include <libavutil/frame.h>
};

class FrameData {
public:
    AVFrame *frame;
    double pts = 0;
    double duration = 0;

    int frameWidth = 0;
    int frameHeight = 0;
    int format = 0;
    int size = 0;
    int nb_samples;

    int linesize[3];    // 宽对齐
    unsigned char *resampleData = 0;
    unsigned char *decodeDatas[8] = {0};
public:
    void release() {

    }

};


#endif //FFPLAYER_FRAMEDATA_H
