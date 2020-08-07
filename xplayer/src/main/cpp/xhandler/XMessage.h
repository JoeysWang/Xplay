//
// Created by 王越 on 2020/7/12.
//

#ifndef XPLAY_XMESSAGE_H
#define XPLAY_XMESSAGE_H

#include <cstdint>

class XHandler;

class XMessage {

public:
    int32_t what = 0;
    int32_t arg1 = 0;
    int64_t arg2 = 0;
    void *obj = nullptr;
    XHandler *target;

};


#endif //XPLAY_XMESSAGE_H
