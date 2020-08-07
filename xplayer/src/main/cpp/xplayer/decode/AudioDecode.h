//
// Created by 王越 on 2020/8/6.
//

#ifndef FFPLAYER_AUDIODECODE_H
#define FFPLAYER_AUDIODECODE_H


#include "IDecode.h"

class AudioDecode : public IDecode {
public:
    AudioDecode();

    void decode() override;

};


#endif //FFPLAYER_AUDIODECODE_H
