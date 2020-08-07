//
// Created by 王越 on 2020/4/26.
//

#ifndef XPLAY_XLOG_H
#define XPLAY_XLOG_H

#include <android/log.h>

class XLog {

};
#define  TAG "xplay"
#define  LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#define  LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG,__VA_ARGS__)
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)


#endif //XPLAY_XLOG_H
