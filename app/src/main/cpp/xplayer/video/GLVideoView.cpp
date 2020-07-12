//
// Created by 王越 on 2020/4/30.
//

#include "GLVideoView.h"
#include "../texture/XTexture.h"
#include "../../XLog.h"
#include "../texture/XEGL.h"

void GLVideoView::setRender(void *view) {
    std::unique_lock<std::mutex> lock(mutex);
    LOGI("GLVideoView::setRender");
    this->view = view;
    noWindowCondition.notify_all();
}

void GLVideoView::render(XData *data) {
    std::unique_lock<std::mutex> lock(mutex);
    if (!view) {
        LOGE("GLVideoView::render view = null");
        noWindowCondition.wait(lock);
    }
    if (!texture) {
        texture = XTexture::create();
        texture->init(view, data->width, data->height);
    }

    if (!data->decodeDatas[0] || data->size == 0) {
        return;
    }
    texture->draw(data->decodeDatas, data->linesize, data->height);
    XEGL::get()->draw();
}

GLVideoView::~GLVideoView() {
    LOGD("~GLVideoView");
    terminate();
}

void GLVideoView::terminate() {
    LOGD("GLVideoView terminate");
    XEGL::get()->terminate();
    delete texture;
    texture = nullptr;
    view = nullptr;
}
