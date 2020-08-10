//
// Created by 王越 on 2020/8/9.
//

#include <XLog.h>
#include "XEGL.h"

void XEGL::init(void *win) {
    std::unique_lock<std::mutex> lock(mutex);
    mWindow = static_cast<ANativeWindow *>(win);
    mSurfaceWidth = ANativeWindow_getWidth(mWindow);
    mSurfaceHeight = ANativeWindow_getHeight(mWindow);

    LOGD("XEGL init mSurfaceWidth=%d , mSurfaceHeight=%d", mSurfaceWidth, mSurfaceHeight);
    //初始化EGL
    //1。获取EGLDisplay设备
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("egl display is null");
        return  ;
    }

    LOGD("egl display  success");

    //2。初始化egl
    if (EGL_TRUE != eglInitialize(display, 0, 0)) {
        LOGE("egl eglInitialize is failed");
        return  ;
    }
    LOGD("egl eglInitialize  success");

    EGLint configSpec[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_NONE
    };
    EGLConfig config[1];
    EGLint numConfigs = 1;
    if (EGL_TRUE != eglChooseConfig(display, configSpec, config, 1, &numConfigs)) {
        LOGE("egl eglChooseConfig is failed");
        return;
    }
    LOGD("egl eglChooseConfig  success");
    eglSurface = eglCreateWindowSurface(display, config[0], mWindow, nullptr);
    if (eglSurface == EGL_NO_SURFACE) {
        LOGE("egl eglSurface is failed");
        switch (eglGetError()) {
            case EGL_BAD_MATCH:
                LOGE("eglSurface EGL_BAD_MATCH");
                break;
            case EGL_BAD_CONFIG:
                LOGE("eglSurface EGL_BAD_CONFIG");
                break;
            case EGL_BAD_NATIVE_WINDOW:
                LOGE("eglSurface EGL_BAD_NATIVE_WINDOW");
                break;
            case EGL_BAD_ALLOC:
                LOGE("eglSurface EGL_BAD_ALLOC");
                break;
            default:
                LOGE("eglSurface unknown");
                break;
        }
        return;
    }
    LOGD("egl eglSurface success!");

    //4 创建打开上下文
    EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
    };
    eglContext = eglCreateContext(display, config[0], EGL_NO_CONTEXT, ctxAttr);
    if (eglContext == EGL_NO_CONTEXT) {
        LOGE("egl eglCreateContext is failed");
        return;
    }
    if (EGL_TRUE != eglMakeCurrent(display, eglSurface, eglSurface, eglContext)) {
        LOGE("egl eglMakeCurrent is failed");
        return;
    }
    LOGD("egl eglMakeCurrent  success");

}

void XEGL::draw() {
    std::unique_lock<std::mutex> lock(mutex);
    if (display == EGL_NO_DISPLAY || eglSurface == EGL_NO_SURFACE) {
        return;
    }
    eglSwapBuffers(display, eglSurface);
}

void XEGL::clear() {
    LOGI("XEGL::clear");
    std::unique_lock<std::mutex> lock(mutex);
    mWindow = nullptr;
    if (eglSurface != EGL_NO_SURFACE) {
        eglDestroySurface(display, eglSurface);
        eglSurface = EGL_NO_SURFACE;
    }
    if (eglContext != EGL_NO_CONTEXT) {
        if (display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        }
        if (eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(display, eglContext);
        }
        if (display != EGL_NO_DISPLAY) {
            eglReleaseThread();
            eglTerminate(display);
        }
        display = EGL_NO_DISPLAY;
        eglContext = EGL_NO_CONTEXT;
    }
}
