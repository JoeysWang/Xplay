//
// Created by ky611 on 2020/6/14.
//

#ifndef XPLAY_IRESAMPLE_H
#define XPLAY_IRESAMPLE_H


#include "../IObserver.h"
#include "../data/XParameter.h"

class IResample : public IObserver {
public:
    virtual bool open(XParameter in, XParameter out = XParameter()) = 0;


    virtual XData resample(XData in) = 0;

    void update(XData data) override;

};


#endif //XPLAY_IRESAMPLE_H
