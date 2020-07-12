//
// Created by 王越 on 2020/7/12.
//

#ifndef XPLAY_XMESSAGEQUEUE_H
#define XPLAY_XMESSAGEQUEUE_H

#include <queue>
#include <mutex>
#include "XMessage.h"

class XMessageQueue {
protected:
    // Data
    std::queue<XMessage> _queue;
    typename std::queue<XMessage>::size_type _size_max;

    // Thread gubbins
    std::mutex _mutex;
    std::condition_variable _fullQue;
    std::condition_variable _empty;

    // Exit
    // 原子操作
    std::atomic_bool _quit; //{ false };
    std::atomic_bool _finished; // { false };
public:
    int serial;
public:
    XMessageQueue(const size_t size_max) : _size_max(size_max) {
        _quit = false;
        _finished = false;
    }

    bool push(XMessage &data) {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_quit && !_finished) {
            if (_queue.size() < _size_max) {
                _queue.push(std::move(data));
                //_queue.push(data);
                _empty.notify_all();
                return true;
            } else {
                // wait的时候自动释放锁，如果wait到了会获取锁
//                LOGE("packet queue is full wait");
                _fullQue.wait(lock);
            }
        }

        return false;
    }

    XMessage *peek() {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_quit) {
            if (!_queue.empty()) {
                return &_queue.front();
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
                return &_queue.back();
            } else if (_queue.empty() && _finished) {
                return nullptr;
            } else {
                return nullptr;
            }
        }
    }

    bool pop(XMessage &data) {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_quit) {
            if (!_queue.empty()) {
                //data = std::move(_queue.front());
                data = _queue.front();
                _queue.pop();

                _fullQue.notify_all();
                return true;
            } else if (_queue.empty() && _finished) {
                return false;
            } else {
//                LOGE("packet queue is empty wait");
                _empty.wait(lock);
            }
        }
        return false;
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
    }

    int length() {
        std::unique_lock<std::mutex> lock(_mutex);
        return static_cast<int>(_queue.size());
    }
};


#endif //XPLAY_XMESSAGEQUEUE_H
