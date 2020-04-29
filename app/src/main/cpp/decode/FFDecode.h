//
// Created by 王越 on 2020/4/29.
//

#ifndef XPLAY_FFDECODE_H
#define XPLAY_FFDECODE_H


#include "IDecode.h"

struct AVCodecContext;
struct AVFrame;

class FFDecode : public IDecode {
public:
    bool open(XParameter parameter) override;

    bool sendPacket(XData *pkt) override;

    XData receiveFrame() override;


protected:
    AVCodecContext *codecContext = 0;
    AVFrame *avFrame = 0;

};


#endif //XPLAY_FFDECODE_H
