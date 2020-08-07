//
// Created by 王越 on 2020/8/6.
//

#ifndef FFPLAYER_VIDEODECODE_H
#define FFPLAYER_VIDEODECODE_H


#include "IDecode.h"

class VideoDecode : public IDecode {
public:
    VideoDecode();

    void decode() override;
};


#endif //FFPLAYER_VIDEODECODE_H
