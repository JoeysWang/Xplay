//
// Created by 王越 on 2020/8/6.
//

#ifndef FFPLAYER_PLAYERSTATE_H
#define FFPLAYER_PLAYERSTATE_H


class PlayerState {
public:
    int abortRequest = 0;               // 退出标志
    int pauseRequest = 1;               // 暂停标志

public:
    virtual ~PlayerState();
};


#endif //FFPLAYER_PLAYERSTATE_H
