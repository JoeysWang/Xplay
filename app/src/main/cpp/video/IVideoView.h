//
// Created by 王越 on 2020/4/30.
//

#ifndef XPLAY_IVIDEOVIEW_H
#define XPLAY_IVIDEOVIEW_H


#include "../data/XData.h"
#include "../IObserver.h"

class IVideoView : public IObserver {

public:
    virtual void setRender(void *view) = 0;

    virtual void render(XData data) = 0;

    virtual void update(XData data);

    virtual void setMatrix(float *matrix, int size) {};

};


#endif //XPLAY_IVIDEOVIEW_H
