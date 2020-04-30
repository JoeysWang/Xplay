//
// Created by 王越 on 2020/4/30.
//
#include "EGL/egl.h"
#include "XEGL.h"
#include "android/native_window_jni.h"
#include "../XLog.h"

class CXEGL : public XEGL {
public:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface eglSurface = EGL_NO_SURFACE;
    EGLContext eglContext = EGL_NO_CONTEXT;

    void draw() override {
        if (display == EGL_NO_DISPLAY || eglSurface == EGL_NO_SURFACE) {
            return;
        }
        eglSwapBuffers(display,eglSurface);
    }

    bool init(void *win) override {
        ANativeWindow *nativeWindow = static_cast<ANativeWindow *>(win);

        //初始化EGL
        //1。获取EGLDisplay设备
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            LOGE("egl display is null");
            return false;
        }

        LOGD("egl display  success");

        //2。初始化egl
        if (EGL_TRUE != eglInitialize(display, 0, 0)) {
            LOGE("egl eglInitialize is failed");
            return false;
        }
        LOGD("egl eglInitialize  success");

        EGLint configSpec[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_NONE
        };
        EGLConfig config = 0;
        EGLint numConfigs = 0;
        if (EGL_TRUE != eglChooseConfig(display, configSpec, &config, 1, &numConfigs)) {
            LOGE("egl eglChooseConfig is failed");
            return false;
        }
        LOGD("egl eglChooseConfig  success");
        eglSurface = eglCreateWindowSurface(display, config, nativeWindow, nullptr);

        //4 创建打开上下文
        EGLint ctxAttr[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
        };
        eglContext = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
        if (eglContext == EGL_NO_CONTEXT) {
            LOGE("egl eglCreateContext is failed");
            return false;
        }
        LOGD("egl eglCreateContext  success");


        if (EGL_TRUE != eglMakeCurrent(display, eglSurface, eglSurface, eglContext)) {
            LOGE("egl eglMakeCurrent is failed");
            return false;

        }
        LOGD("egl eglMakeCurrent  success");


        return true;
    }

};


XEGL *XEGL::get() {
    static CXEGL egl;
    return &egl;
}
