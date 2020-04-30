//
// Created by 王越 on 2020/4/26.
//

#ifndef XPLAY_XDATA_H
#define XPLAY_XDATA_H

struct AVPacket;

struct XData {

    unsigned char *data = 0;
    //解码后的数据
    unsigned char *datas[8] = {0};

    int size = 0;

    void Drop();

    //audio 0,video 1
    int audioOrVideo = -1;

    int width = 0;
    int height = 0;
};


#endif //XPLAY_XDATA_H
