add_library( xhandler
        SHARED
        # library
        XHandler.cpp
         )
# 链接静态库
target_link_libraries(
        xhandler
        android
        xlog )