//
// Created by ky611 on 2020/6/16.
//

#ifndef XPLAY_IAUDIOPLAY_H
#define XPLAY_IAUDIOPLAY_H


#include "../IObserver.h"
#include "../data/XParameter.h"

class IAudioPlay : public IObserver {
public:
    void update(XData data) override;
    virtual bool startPlay(XParameter out) = 0;

};


#endif //XPLAY_IAUDIOPLAY_H
