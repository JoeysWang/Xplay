package com.joeys.xplay.metadata

import android.util.Log
import androidx.annotation.IntegerRes
import java.util.*

class MediaMetadataRetriever {
    companion object {
        init {
            System.loadLibrary("metadata_retriever")
            native_init()
        }

        external fun native_init()
    }

    var mNativeContext: Long = 0

    init {
        native_setup()
        Log.i(
            "xplay",
            "MediaMetadataRetriever init  ${java.lang.Long.toHexString(mNativeContext)}"
        );
    }


    external fun native_setup()

    external fun setDataSource(url: String)

    external fun release()

    fun getMetadata(): XMetadata? {
        val hashMap: HashMap<String, String> = _getAllMetadata()
        if (hashMap != null) {
            return XMetadata(hashMap)
        }
        return null
    }

    private external fun _getAllMetadata(): HashMap<String, String>
}