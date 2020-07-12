add_library(
        xhandler
        SHARED
        # library
        XHandler.cpp
        LooperManager.cpp
        XMessageQueue.cpp
        XMessage.cpp
        XLooper.cpp
        ThreadUtils.cpp
        HandlerThread.cpp
)
# 链接静态库
target_link_libraries(
        xhandler
        android
        log)