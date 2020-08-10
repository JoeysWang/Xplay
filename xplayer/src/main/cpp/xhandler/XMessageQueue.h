//
// Created by 王越 on 2020/7/12.
//

#ifndef XPLAY_XMESSAGEQUEUE_H
#define XPLAY_XMESSAGEQUEUE_H

#include <queue>
#include <mutex>
#include <XLog.h>
#include "XMessage.h"

class XMessageQueue {
protected:
    // Data
    std::queue<XMessage *> _queue;
    typename std::queue<XMessage *>::size_type _size_max;

    // Thread gubbins
    std::mutex _mutex;
    std::condition_variable _fullQue;
    std::condition_variable _empty;

    // Exit
    // 原子操作
    std::atomic_bool _quit{false}; //{ false };
    std::atomic_bool _finished{false}; // { false };
public:
    int serial;
public:
    XMessageQueue(const size_t size_max) : _size_max(size_max) {
        _quit = false;
        _finished = false;
    }

    bool push(XMessage *data) {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_quit && !_finished) {
            if (_queue.size() < _size_max) {
                _queue.push(data);
                _empty.notify_all();
                return true;
            } else {
                LOGE("XMessageQueue is full wait");
                _fullQue.wait(lock);
                LOGE("XMessageQueue full awake");
            }
        }
        LOGE("XMessageQueue is _quit ,push  return;");

        return false;
    }

    XMessage *peek() {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_quit) {
            if (!_queue.empty()) {
                return _queue.front();
            } else if (_queue.empty() && _finished) {
                return nullptr;
            } else {
                return nullptr;
            }
        }
    }

    XMessage *last() {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_quit) {
            if (!_queue.empty()) {
                return _queue.back();
            } else if (_queue.empty() && _finished) {
                return nullptr;
            } else {
                return nullptr;
            }
        }
    }

    XMessage *pop() {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_quit) {
            if (!_queue.empty()) {
                //data = std::move(_queue.front());
                XMessage *data = _queue.front();
                _queue.pop();
                _fullQue.notify_all();
                return data;
            } else if (_queue.empty() && _finished) {
                return nullptr;
            } else {
                LOGE("XMessageQueue is empty wait");
                _empty.wait(lock);
                LOGE("XMessageQueue empty awake");
            }
        }
        LOGE("XMessageQueue is _quit ,pop return;");
        return nullptr;
    }

    // The queue has finished accepting input
    void finished() {
        _finished = true;
        _empty.notify_all();
    }

    bool isFinish() {
        return _finished;
    }

    void quit() {
        _quit = true;
        _empty.notify_all();
        _fullQue.notify_all();
        clear();
        LOGE("XMessageQueue quit notify_all");
    }

    void clear() {
        std::queue<XMessage *> empty;
        std::swap(empty, _queue);
    }

    int length() {
        std::unique_lock<std::mutex> lock(_mutex);
        return static_cast<int>(_queue.size());
    }
};


#endif //XPLAY_XMESSAGEQUEUE_H
