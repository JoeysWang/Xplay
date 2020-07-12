add_library(
        metadata_retriever
        SHARED
        # library
        MediaMetadataRetriever.cpp
        Metadata.cpp
        com_joeys_MediaMetadataRetriever.cpp
        )
# 链接静态库
target_link_libraries(
        metadata_retriever
        ${ffmpeg_libs_names}
        android
        jnigraphics
        log)