//
// Created by 王越 on 2020/4/26.
//

#ifndef XPLAY_XDATA_H
#define XPLAY_XDATA_H

struct AVPacket;
enum XDataType {
    AVPACKET_TYPE = 0,
    UCHAR_TYPE = 1,

};

struct XData {

    unsigned char *data = 0;
    //解码后的数据
    unsigned char *datas[8] = {0};

    int size = 0;

    void drop();

    bool alloc(int size, const char *data = 0);

    //audio 0,video 1
    int audioOrVideo = -1;

    int width = 0;
    int height = 0;
    int linesize[3];    // 宽对齐

    int type = 0;
};


#endif //XPLAY_XDATA_H
