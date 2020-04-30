//
// Created by 王越 on 2020/4/30.
//

#include "GLVideoView.h"
#include "../texture/XTexture.h"
#include "../XLog.h"

void GLVideoView::setRender(void *view) {
    this->view = view;
}

void GLVideoView::render(XData data) {
    if (!view)return;
    if (!texture) {
        texture = XTexture::create();
        texture->init(view);
    }
    LOGD("render %d", data.size);

    try {
        texture->draw(data.datas, data.width, data.height);
    } catch (const char *&e) {  //exception类位于<exception>头文件中
        LOGE("render error %s", e);
    }
}
