//
// Created by 王越 on 2020/8/9.
//

#include "IVideoView.h"

IVideoView::IVideoView(const std::shared_ptr<PlayerState> &playerState) : playerState(
        playerState) {}

void IVideoView::quit() {

}
