//
// Created by 王越 on 2020/4/26.
//

#ifndef XPLAY_XDATA_H
#define XPLAY_XDATA_H

extern "C" {
#include "libavutil/rational.h"
#include "libavformat/avformat.h"
};
struct AVPacket;
enum XDataType {
    AVPACKET_TYPE = 0,
    UCHAR_TYPE = 1,

};

struct XData {

    unsigned char *resampleData = 0;
    unsigned char *decodeDatas[8] = {0};

    AVFrame *frame = 0;
    AVPacket *packet = 0;


    int size = 0;

    void drop();

    bool alloc(int size, const char *data = 0);

    //audio 0,video 1
    int audioOrVideo = -1;

    AVRational time_base;
    AVRational frame_rate;
    double pts = 0;
    double duration = 0;

    int width = 0;
    int height = 0;
    int format = 0;
    int nb_samples;
    int linesize[3];    // 宽对齐


    int allocType = 0;
};


#endif //XPLAY_XDATA_H
