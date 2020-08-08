//
// Created by 王越 on 2020/8/6.
//

#include <XLog.h>
#include "AudioDecode.h"

AudioDecode::AudioDecode() {
    LOGI("AudioDecode::AudioDecode");
    packetQueue->tag = "Audio";
}

void AudioDecode::decode() {

}
