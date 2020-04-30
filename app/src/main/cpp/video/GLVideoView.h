//
// Created by 王越 on 2020/4/30.
//

#ifndef XPLAY_GLVIDEOVIEW_H
#define XPLAY_GLVIDEOVIEW_H


#include "IVideoView.h"

class XTexture;

class GLVideoView : public IVideoView {
public:
    void setRender(void *view) override;

    void render(XData data) override;

protected:
    void *view = 0;
    XTexture *texture = 0;
};


#endif //XPLAY_GLVIDEOVIEW_H