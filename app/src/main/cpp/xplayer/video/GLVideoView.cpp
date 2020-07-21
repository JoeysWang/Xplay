//
// Created by 王越 on 2020/4/30.
//

#include "GLVideoView.h"
#include "../texture/XTexture.h"
#include "../../XLog.h"
#include "../texture/XEGL.h"

const char *GLVideoView::formatString(int formate) {
    switch (formate) {
        case AV_PIX_FMT_NONE:
            return "AV_PIX_FMT_NONE";
        case AV_PIX_FMT_YUV420P:
            return "AV_PIX_FMT_YUV420P";
        case AV_PIX_FMT_YUYV422:
            return "AV_PIX_FMT_YUYV422";
        case AV_PIX_FMT_RGB24:
            return "AV_PIX_FMT_RGB24";
        case AV_PIX_FMT_BGR24:
            return "AV_PIX_FMT_BGR24";
        case AV_PIX_FMT_YUV422P:
            return "AV_PIX_FMT_YUV422P";
        case AV_PIX_FMT_YUV444P:
            return "AV_PIX_FMT_YUV444P";
        case AV_PIX_FMT_YUV410P:
            return "AV_PIX_FMT_YUV410P";
        case AV_PIX_FMT_YUV411P:
            return "AV_PIX_FMT_YUV411P";
        case AV_PIX_FMT_GRAY8:
            return "AV_PIX_FMT_GRAY8";
        case AV_PIX_FMT_MONOWHITE:
            return "AV_PIX_FMT_MONOWHITE";
        case AV_PIX_FMT_MONOBLACK:
            return "AV_PIX_FMT_MONOBLACK";
        case AV_PIX_FMT_PAL8:
            return "AV_PIX_FMT_PAL8";
        case AV_PIX_FMT_YUVJ420P:
            return "AV_PIX_FMT_YUVJ420P";
        case AV_PIX_FMT_YUVJ422P:
            return "AV_PIX_FMT_YUVJ422P";
        case AV_PIX_FMT_YUVJ444P:
            return "AV_PIX_FMT_YUVJ444P";
        case AV_PIX_FMT_UYVY422:
            return "AV_PIX_FMT_UYVY422";
        case AV_PIX_FMT_UYYVYY411:
            return "AV_PIX_FMT_UYYVYY411";
        case AV_PIX_FMT_BGR8:
            return "AV_PIX_FMT_BGR8";
        case AV_PIX_FMT_BGR4:
            return "AV_PIX_FMT_BGR4";
        case AV_PIX_FMT_BGR4_BYTE:
            return "AV_PIX_FMT_BGR4_BYTE";
        case AV_PIX_FMT_RGB8:
            return "AV_PIX_FMT_RGB8";
        case AV_PIX_FMT_RGB4:
            return "AV_PIX_FMT_RGB4";
        case AV_PIX_FMT_RGB4_BYTE:
            return "AV_PIX_FMT_RGB4_BYTE";
        case AV_PIX_FMT_NV12:
            return "AV_PIX_FMT_NV12";
        case AV_PIX_FMT_NV21:
            return "AV_PIX_FMT_NV21";
        case AV_PIX_FMT_ARGB:
            return "AV_PIX_FMT_ARGB";
        case AV_PIX_FMT_RGBA:
            return "AV_PIX_FMT_RGBA";
        case AV_PIX_FMT_ABGR:
            return "AV_PIX_FMT_ABGR";
        case AV_PIX_FMT_BGRA:
            return "AV_PIX_FMT_BGRA";
        default:
            return formate + "";
    }
}

void GLVideoView::setRender(void *view) {
    std::unique_lock<std::mutex> lock(mutex);
    LOGI("GLVideoView::setRender %p", view);
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
        texture->init(view, data->frameWidth, data->frameHeight);
    }
    if (!data->decodeDatas[0] || data->size == 0) {
        return;
    }
    LOGI("format = %s,linesize = [%d , %d , %d] ",
         formatString(data->format),
         data->linesize[0],
         data->linesize[1],
         data->linesize[2]
    );
    texture->draw(data->decodeDatas, data->linesize, data->frameHeight);
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
