//
// Created by ky611 on 2020/6/20.
//

#include "IPlayerProxy.h"
#include "../builder/FFPlayerBuilder.h"

void IPlayerProxy::init() {
    if (player == nullptr) {
        mutex.lock();
        if (player == nullptr) {
            player = FFPlayerBuilder::get()->buildPlayer();
        }
        mutex.unlock();
    }
}

bool IPlayerProxy::open(const char *path) {
    mutex.lock();
    if (player != nullptr) {
        player->open(path);
    }
    mutex.unlock();
    return true;
}

bool IPlayerProxy::initView(void *window) {
    mutex.lock();
    if (player != nullptr) {
        player->initView(window);
    }
    mutex.unlock();
    return true;
}

void IPlayerProxy::start() {
    mutex.lock();
    if (player != nullptr) {
        player->start();
    }
    mutex.unlock();
}

