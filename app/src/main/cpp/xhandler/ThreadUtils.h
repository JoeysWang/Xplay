//
// Created by 王越 on 2020/7/12.
//

#ifndef XPLAY_THREADUTILS_H
#define XPLAY_THREADUTILS_H

#include <thread>
#include <iosfwd>
#include <iostream>

class ThreadUtils {
public:
    static long currentId() {
        std::thread::id id = std::this_thread::get_id();
        long tid = std::hash<std::thread::id>()(id);
        return tid;
    }
};


#endif //XPLAY_THREADUTILS_H
