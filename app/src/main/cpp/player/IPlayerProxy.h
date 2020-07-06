//
// Created by ky611 on 2020/6/20.
//

#ifndef XPLAY_IPLAYERPROXY_H
#define XPLAY_IPLAYERPROXY_H


#include <mutex>
#include "IPlayer.h"

class IPlayerProxy : public IPlayer {

public:
    void init();

    static IPlayerProxy *get() {
        static IPlayerProxy proxy;
        return &proxy;
    }

    bool open(const char *path) override;

    bool initView(void *window) override;

    void start() override;


protected:

protected:
    IPlayerProxy() {};
    IPlayer *player = nullptr;
    std::mutex mutex;
};


#endif //XPLAY_IPLAYERPROXY_H
