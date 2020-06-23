//
// Created by 王越 on 2020/4/30.
//

#include "GLVideoView.h"
#include "../texture/XTexture.h"
#include "../XLog.h"
#include "../texture/XEGL.h"

void GLVideoView::setRender(void *view) {
    this->view = view;
}

void GLVideoView::render(XData data) {

    if (!view) {
        LOGE("GLVideoView::render view = null");
        return;
    }
    if (!texture) {
        texture = XTexture::create();
        texture->init(view, data.width, data.height);
    }

    if (!data.frame->data){

    }
    texture->draw(data.frame->data, data.linesize, data.height);
    XEGL::get()->draw();
}

void GLVideoView::setMatrix(float *m, int size) {
    LOGD("GLVideoView::setMatrix");
    for (int i = 0; i < size; ++i) {
        matrix[i] = *(m + i);
        LOGD("setMatrix  matrix[%d]= %f", i, matrix[i]);
    }
}
