//
// Created by 王越 on 2020/4/26.
//

#ifndef XPLAY_XDATA_H
#define XPLAY_XDATA_H

struct AVPacket;

struct XData {

    unsigned char *data = 0;

    int size = 0;

    void Drop();

    bool isAudio = false;
};


#endif //XPLAY_XDATA_H
