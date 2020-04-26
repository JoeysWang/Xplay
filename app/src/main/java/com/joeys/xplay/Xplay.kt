package com.joeys.xplay

object Xplay {

    init {
        System.loadLibrary("xplay")

    }


    external  fun open(url:String):Boolean

}