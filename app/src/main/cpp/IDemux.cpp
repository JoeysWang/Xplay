//
// Created by 王越 on 2020/4/26.
//

#include "IDemux.h"
#include "XLog.h"

void IDemux::Main() {
    while (!isExit) {
        XData d = read();
//        LOGD("IDemux Read %d", d.size);
//        if (d.size <= 0)break;
    }
}
