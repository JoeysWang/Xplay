package com.joeys.xplay

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class Xplay : GLSurfaceView, SurfaceHolder.Callback, GLSurfaceView.Renderer {
    constructor(context: Context) : super(context)
    constructor(context: Context, attributeSet: AttributeSet) : super(context, attributeSet)

    init {
        System.loadLibrary("xplay")

    }

    external fun open(url: String): Boolean

    override fun surfaceCreated(holder: SurfaceHolder?) {
        Log.d("xplay", "surfaceCreated")
        //初始化open gl egl显示
        initView(holder?.surface)
        setRenderer(this)
    }


    private external fun initView(holder: Surface?)


    override fun surfaceChanged(
        holder: SurfaceHolder?, format: Int, width: Int,
        height: Int
    ) {

    }


    override fun surfaceDestroyed(holder: SurfaceHolder?) {

    }

    override fun onDrawFrame(gl: GL10?) {
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {

    }
}