//
// Created by 王越 on 2020/7/3.
//

#ifndef XPLAY_AUDIODECODE_H
#define XPLAY_AUDIODECODE_H


#include "IDecode.h"

class AudioDecode : public IDecode {
public:
    AudioDecode(PlayerState *playerState);

    int decodePacket() override;

    void start() override;

};


#endif //XPLAY_AUDIODECODE_H
