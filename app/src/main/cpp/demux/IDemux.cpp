//
// Created by 王越 on 2020/4/26.
//

#include "IDemux.h"
#include "../XLog.h"

void IDemux::Main() {
    while (!isExit) {
        XData d = read();
        if (d.size > 0)
            notify(d);
    }
}
