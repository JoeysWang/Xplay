//
// Created by 王越 on 2020/4/30.
//

#include "../../XLog.h"
#include "XEGL.h"
#include "../../xhandler/ThreadUtils.h"

class CXEGL : public XEGL {
public:
    void clear() override {
        LOGD("CXEGL::clear thread id=%ld", ThreadUtils::currentId());

        mutex.lock();
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
        mutex.unlock();
    }



    void draw() override {
        if (display == EGL_NO_DISPLAY || eglSurface == EGL_NO_SURFACE) {
            return;
        }
        mutex.lock();
        eglSwapBuffers(display, eglSurface);
        mutex.unlock();
    }

    bool init(void *win) override {
        std::unique_lock<std::mutex> lock(mutex);
        mWindow = static_cast<ANativeWindow *>(win);
        mSurfaceWidth = ANativeWindow_getWidth(mWindow);
        mSurfaceHeight = ANativeWindow_getHeight(mWindow);
//        if (mSurfaceWidth != 0 && mSurfaceHeight != 0) {
//            // 宽高比例不一致时，需要调整缓冲区的大小，这里是以宽度为基准
//            if ((mSurfaceWidth / mSurfaceHeight) != (width / height)) {
//                mSurfaceHeight = mSurfaceWidth * height / width;
//                int windowFormat = ANativeWindow_getFormat(mWindow);
//
//                ANativeWindow_setBuffersGeometry(mWindow, mSurfaceWidth, mSurfaceHeight,
//                                                 windowFormat);
//            }
//        }

        LOGD("CXEGL init mSurfaceWidth=%d , mSurfaceHeight=%d", mSurfaceWidth, mSurfaceHeight);
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
        EGLConfig config[1];
        EGLint numConfigs = 1;
        if (EGL_TRUE != eglChooseConfig(display, configSpec, config, 1, &numConfigs)) {
            LOGE("egl eglChooseConfig is failed");
            return false;
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
            return false;
        }
        LOGD("egl eglSurface success!");

        //4 创建打开上下文
        EGLint ctxAttr[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
        };
        eglContext = eglCreateContext(display, config[0], EGL_NO_CONTEXT, ctxAttr);
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
