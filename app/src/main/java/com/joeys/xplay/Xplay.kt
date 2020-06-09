package com.joeys.xplay

import android.content.Context
import android.opengl.GLSurfaceView
import android.opengl.Matrix
import android.util.AttributeSet
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import java.util.*
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

    private val vPMatrix = FloatArray(16)
    private val projectionMatrix = FloatArray(16)
    private val viewMatrix = FloatArray(16)

    override fun surfaceChanged(
        holder: SurfaceHolder?,
        format: Int, width: Int,
        height: Int
    ) {
        Log.d("xplay", "surfaceChanged: $width/$height")
        val ratio: Float = width.toFloat() / height.toFloat()
        Matrix.frustumM(projectionMatrix, 0, -ratio, ratio, -1f, 1f, 1.0f, 20f)

        Matrix.setLookAtM(
            viewMatrix, 0,
            0f, 0f, -3f,
            0f, 0f, 0f,
            0f, 1.0f, 0.0f
        )
        Matrix.multiplyMM(vPMatrix, 0, projectionMatrix, 0, viewMatrix, 0)

        Log.d("xplay", "vPMatrix: ${vPMatrix.contentToString()}")
        setMatrix(vPMatrix)

    }

    external fun setMatrix(vPMatrix: FloatArray)


    override fun surfaceDestroyed(holder: SurfaceHolder?) {

    }

    override fun onDrawFrame(gl: GL10?) {
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {

    }
}