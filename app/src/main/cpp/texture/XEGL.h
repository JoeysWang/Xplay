//
// Created by 王越 on 2020/4/30.
//

#ifndef XPLAY_XEGL_H
#define XPLAY_XEGL_H

#include "android/native_window_jni.h"
#include "android/native_window.h"
#include "EGL/egl.h"
class XEGL {
public:
    virtual bool init(void *win,int width, int height) = 0;
    virtual void draw( ) = 0;

    static XEGL *get();

protected:
    XEGL(){}
    ANativeWindow *mWindow;             // Surface窗口
    int mSurfaceWidth;                  // 窗口宽度
    int mSurfaceHeight;                 // 窗口高度
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface eglSurface = EGL_NO_SURFACE;
    EGLContext eglContext = EGL_NO_CONTEXT;
};



#endif //XPLAY_XEGL_H
