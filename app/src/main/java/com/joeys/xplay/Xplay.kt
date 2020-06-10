package com.joeys.xplay

import android.content.Context
import android.graphics.SurfaceTexture
import android.util.AttributeSet
import android.util.Log
import android.view.Surface
import android.view.TextureView
import kotlin.concurrent.thread


class Xplay : TextureView, TextureView.SurfaceTextureListener {
    constructor(context: Context) : super(context)
    constructor(context: Context, attributeSet: AttributeSet) : super(context, attributeSet)

//    private var mRenderer: IGLESRenderer? = null
//    private val mGLThread: GLESTVThread? = null

    init {
        System.loadLibrary("xplay")
        surfaceTextureListener = this;
    }
//
//    fun setRenderer(renderer: IGLESRenderer) {
//        mRenderer = renderer
//    }
//    fun requestRender() {
//        mGLThread.requestRender()
//    }

    external fun open(url: String): Boolean

    private external fun initView(holder: Surface?)

    external fun setMatrix(vPMatrix: FloatArray)

    override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture?, width: Int, height: Int) {
        Log.d("xplay", "surfaceCreated")

    }

    override fun onSurfaceTextureUpdated(surface: SurfaceTexture?) {

    }

    override fun onSurfaceTextureDestroyed(surface: SurfaceTexture?): Boolean {
        return false;
    }

    override fun onSurfaceTextureAvailable(surface: SurfaceTexture?, width: Int, height: Int) {
        Log.d("xplay", "onSurfaceTextureAvailable")
        thread {
            initView(Surface(surface))
        }
    }


//    private val vPMatrix = FloatArray(16)
//    private val projectionMatrix = FloatArray(16)
//    private val viewMatrix = FloatArray(16)
//
//    override fun surfaceCreated(holder: SurfaceHolder?) {
//        Log.d("xplay", "surfaceCreated")
//        //初始化open gl egl显示
//        initView(holder?.surface)
//
//    }
//    override fun surfaceChanged(
//        holder: SurfaceHolder?,
//        format: Int, width: Int,
//        height: Int
//    ) {
//        Log.d("xplay", "surfaceChanged: $width/$height")
//        val ratio: Float = width.toFloat() / height.toFloat()
//        Matrix.frustumM(projectionMatrix, 0, -ratio, ratio, -1f, 1f, 1.0f, 20f)
//
//        Matrix.setLookAtM(
//            viewMatrix, 0,
//            0f, 0f, -3f,
//            0f, 0f, 0f,
//            0f, 1.0f, 0.0f
//        )
//        Matrix.multiplyMM(vPMatrix, 0, projectionMatrix, 0, viewMatrix, 0)
//
//        Log.d("xplay", "vPMatrix: ${vPMatrix.contentToString()}")
//        setMatrix(vPMatrix)
//
//    }

    //    override fun surfaceDestroyed(holder: SurfaceHolder?) {
//
//    }
}