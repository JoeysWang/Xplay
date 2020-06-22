//
// Created by ky611 on 2020/6/16.
//

#ifndef XPLAY_IAUDIOPLAY_H
#define XPLAY_IAUDIOPLAY_H


#include <list>
#include "../IObserver.h"
#include "../data/XParameter.h"

class IAudioPlay : public IObserver {
public:
    //缓冲满后会阻塞
    void update(XData data) override;

    virtual bool startPlay(XParameter out) = 0;

    //获取缓冲数据，如没有就阻塞
    XData getData();

public:
    int maxFrameBuffer = 100;
    int pts;

protected:
    std::list<XData> frames;
    std::mutex framesMutex;
    std::condition_variable notEmpty;
    std::condition_variable notFull;
};


#endif //XPLAY_IAUDIOPLAY_H
