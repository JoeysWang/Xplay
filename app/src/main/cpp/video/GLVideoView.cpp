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
    if (!view)return;
    if (!texture) {
        texture = XTexture::create();
        texture->init(view, matrix);
    }
    texture->draw(data.datas, data.linesize, data.height);
    XEGL::get()->draw();
}

void GLVideoView::setMatrix(float m[], int size) {
    this->matrix = m;
}
