//
// Created by 王越 on 2020/8/6.
//

#include <XLog.h>
#include "VideoDecode.h"

VideoDecode::VideoDecode() {
    LOGI("VideoDecode::VideoDecode");
    packetQueue->tag = "Video";
}

void VideoDecode::decode() {

}
