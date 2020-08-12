//
// Created by 王越 on 2020/8/9.
//

#include <XLog.h>
#include "GLVideoView.h"

GLVideoView::GLVideoView(const std::shared_ptr<PlayerState> &playerState) : IVideoView(
        playerState) {
    xegl = std::make_shared<XEGL>();
}

void GLVideoView::setRenderSurface(void *win) {
    std::unique_lock<std::mutex> lock(mutex);
    this->renderSurface = win;
    noWindowCondition.notify_all();
}

void GLVideoView::render(FrameData *data) {
    std::unique_lock<std::mutex> lock(mutex);
    if (!renderSurface) {
        LOGE("GLVideoView::render view = null");
        noWindowCondition.wait(lock);
    }
    if (!texture) {
        texture = std::make_unique<XTexture>(xegl);
        texture->init(renderSurface);
    }
    texture->setFrameWidthHeight(data->frameWidth,
                                 data->frameHeight);
    if (!data->decodeDatas[0] || data->size == 0) {
        return;
    }
//    LOGI("GLVideoView::render \nformat = %s,linesize = [%d , %d , %d] ",
//         formatString(data->format),
//         data->linesize[0],
//         data->linesize[1],
//         data->linesize[2]
//    );
    texture->draw(data->decodeDatas, data->linesize, data->frameHeight);
    xegl->draw();
}

void GLVideoView::quit() {
    LOGI("GLVideoView::quit");
    xegl->clear();
}

GLVideoView::~GLVideoView() {
    LOGI("~GLVideoView");
}
