//
// Created by 王越 on 2020/8/9.
//

#ifndef XPLAY_IVIDEOVIEW_H
#define XPLAY_IVIDEOVIEW_H


#include "../data/FrameData.h"
#include "../data/PlayerState.h"
#include <thread>

class IVideoView {
public:
    IVideoView(const std::shared_ptr<PlayerState> &playerState);

    virtual void quit() = 0;
    virtual void render(FrameData *frameData) = 0;
    virtual void setRenderSurface(void *win) = 0;

protected:
    std::mutex mutex;
    std::shared_ptr<PlayerState> playerState;

};


#endif //XPLAY_IVIDEOVIEW_H
