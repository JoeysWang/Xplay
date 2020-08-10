//
// Created by 王越 on 2020/8/9.
//

#ifndef XPLAY_GLVIDEOVIEW_H
#define XPLAY_GLVIDEOVIEW_H

#include <memory>
#include "texture/XEGL.h"
#include "IVideoView.h"
#include "texture/XTexture.h"

class GLVideoView : public IVideoView {

public:
    GLVideoView(const std::shared_ptr<PlayerState> &playerState);

    virtual ~GLVideoView();

private:
    void setRenderSurface(void *win) override;

    void render(FrameData *frameData) override;

    void quit() override;


protected:
    std::shared_ptr<XEGL> xegl;
    std::unique_ptr<XTexture> texture;
    std::condition_variable noWindowCondition;

    void *renderSurface = 0;

    const char *formatString(int formate) {
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

};


#endif //XPLAY_GLVIDEOVIEW_H
