//
// Created by 王越 on 2020/8/6.
//

#ifndef FFPLAYER_MEDIASYNC_H
#define FFPLAYER_MEDIASYNC_H


#include <memory>
#include "../data/PlayerState.h"
#include "XLog.h"

class MediaSync {
public:
    MediaSync(std::shared_ptr<PlayerState> const &state) {
        playerState = state;
    }

    virtual ~MediaSync() {
        LOGD("~MediaSync");
    }

protected:
    std::shared_ptr<PlayerState> playerState;


};


#endif //FFPLAYER_MEDIASYNC_H
