//
// Created by 王越 on 2020/7/12.
//

#ifndef XPLAY_LOOPERMANAGER_H
#define XPLAY_LOOPERMANAGER_H
#include <memory>

#include <mutex>
#include <map>
#include "XLooper.h"
#include "XLog.h"

class LooperManager {

private:
    LooperManager() {}


public:
    static LooperManager *getInstance() {
        if (!instance) { instance = new LooperManager(); }
        return instance;
    }

    std::shared_ptr<XLooper> createLooper(long threadId) {
        LOGI(" XLooper *createLooper threadId=%ld", threadId);
        std::unique_lock<std::mutex> lock(mutex);
        auto finder = looperMap.find(threadId);
        if (finder == looperMap.end()) {
            auto newLooper = std::make_shared<XLooper>();
            looperMap[threadId] = newLooper;
            return newLooper;
        }
        return nullptr;
    }

    std::shared_ptr<XLooper> getLooper(long threadId) {
        LOGI(" XLooper *getLooper threadId=%ld", threadId);
        std::unique_lock<std::mutex> lock(mutex);
        auto finder = looperMap.find(threadId);
        if (finder == looperMap.end()) {
            return nullptr;
        }
        return finder->second;
    }

    void deleteLooper(long threadId) {
        std::unique_lock<std::mutex> lock(mutex);
        auto finder = looperMap.find(threadId);
        if (finder == looperMap.end()) {
            return;
        }

        looperMap.erase(finder);
    }

private:
    static LooperManager *instance;
    std::mutex mutex;
    std::map<long, std::shared_ptr<XLooper>> looperMap;

};


#endif //XPLAY_LOOPERMANAGER_H
