package com.joeys.xplay.metadata

import java.util.*

class MediaMetadataRetriever {
//    private val mNativeContext: Long = 0
    init {
        System.loadLibrary("metadata_retriever")
        native_setup()
    }

    external fun native_setup()

    external fun setDataSource(url: String)

    fun getMetadata(): XMetadata? {
        val hashMap: HashMap<String, String> = _getAllMetadata()
        if (hashMap != null) {
            return XMetadata(hashMap)
        }
        return null
    }

    private external fun _getAllMetadata(): HashMap<String, String>
}