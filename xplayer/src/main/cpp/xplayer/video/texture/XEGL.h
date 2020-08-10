//
// Created by 王越 on 2020/8/9.
//

#ifndef XPLAY_XEGL_H
#define XPLAY_XEGL_H

#include <thread>
#include <android/native_window.h>
#include "EGL/egl.h"

class XEGL {
public:
    void init(void *win);

    virtual void draw();

    virtual void clear();


protected:
    std::mutex mutex;
    ANativeWindow *mWindow;             // Surface窗口
    int mSurfaceWidth;                  // 窗口宽度
    int mSurfaceHeight;                 // 窗口高度
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface eglSurface = EGL_NO_SURFACE;
    EGLContext eglContext = EGL_NO_CONTEXT;

};


#endif //XPLAY_XEGL_H
