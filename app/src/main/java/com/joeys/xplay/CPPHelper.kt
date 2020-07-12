package com.joeys.xplay

class CPPHelper {
    companion object {
        init {
            System.loadLibrary("cpphelper")
        }
    }

    external fun cpptest()
}