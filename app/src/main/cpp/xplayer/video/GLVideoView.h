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

    void render(XData *data) override;

    const char *formatString(int formate);

    virtual ~GLVideoView();


protected:
    void *view = 0;
    XTexture *texture = 0;
    float matrix[16];

    std::condition_variable noWindowCondition;
public:
    void terminate() override;
};


#endif //XPLAY_GLVIDEOVIEW_H
