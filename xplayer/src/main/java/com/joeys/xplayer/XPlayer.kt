package com.joeys.xplayer

import android.content.Context
import android.graphics.SurfaceTexture
import android.net.Uri
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.AttributeSet
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import android.view.TextureView
import com.blankj.utilcode.util.UriUtils
import com.joeys.xplayer.IMediaPlayer.*
import java.io.FileDescriptor
import java.lang.ref.WeakReference
import kotlin.concurrent.thread


class XPlayer : TextureView, TextureView.SurfaceTextureListener, IMediaPlayer {
    private var mEventHandler: EventHandler?=null

    private var mOnPreparedListener: OnPreparedListener? = null
    private var mOnBufferingUpdateListener: OnBufferingUpdateListener? = null
    private var mOnCompletionListener: OnCompletionListener? = null
    private var mOnSeekCompleteListener: OnSeekCompleteListener? = null
    private var mOnErrorListener: OnErrorListener? = null
    private var mOnInfoListener: OnInfoListener? = null
    private var mOnVideoSizeChangedListener: OnVideoSizeChangedListener? = null

    var mNativeContext: Long = 0

    companion object {

        private val TAG = "xplay" // interface test message
        private val MEDIA_NOP = 0 // interface test message

        private val MEDIA_PREPARED = 0x1
        private val MEDIA_PLAYBACK_COMPLETE = 0x2
        private val MEDIA_BUFFERING_UPDATE = 0x3
        private val MEDIA_SEEK_COMPLETE = 0x4
        private val MEDIA_SET_VIDEO_SIZE = 0x5
        private val MEDIA_TIMED_TEXT = 0x99
        private val MEDIA_ERROR = 0x100
        private val MEDIA_INFO = 0x200
        private val MEDIA_CURRENT = 0x300

        init {
            System.loadLibrary("xplay")
            nativeInit()
        }

        @JvmStatic
        external fun nativeInit()

        @JvmStatic
        fun postEventFromNative(
            mediaplayer_ref: Any,
            what: Int, arg1: Int, arg2: Int, obj: Any?
        ) {
            val mp: XPlayer = (mediaplayer_ref as WeakReference<*>).get() as XPlayer? ?: return
            mp.mEventHandler?.let {
                val m: Message = it.obtainMessage(what, arg1, arg2, obj)
                it.sendMessage(m)
            }
        }
    }

    constructor(context: Context) : super(context)
    constructor(context: Context, attributeSet: AttributeSet) : super(context, attributeSet)

    init {
//        val looper = Looper.myLooper()
//
//        when {
//            looper != null -> {
//                mEventHandler = EventHandler(this, looper)
//            }
//            else -> {
//                mEventHandler = EventHandler(this, Looper.getMainLooper())
//            }
//        }
        initPlayer(WeakReference(this))
        surfaceTextureListener = this
    }

    private external fun initPlayer(weakReference: WeakReference<XPlayer>)
    private external fun setDataSourceInternal(url: String)
    private external fun initView(holder: Surface?)

    override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture?, width: Int, height: Int) {
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

    private fun stayAwake(awake: Boolean) {

    }


    private inner class EventHandler(val mMediaPlayer: XPlayer, looper: Looper?) : Handler(looper) {

        override fun handleMessage(msg: Message) {
            if (mMediaPlayer.mNativeContext == 0L) {
                Log.w(
                    XPlayer.TAG,
                    "mediaplayer went away with unhandled events"
                )
                return
            }
//            Log.w(
//                com.joeys.xplayer.Xplay.TAG,
//                "EventHandler handleMessage what=${msg.what} arg1=${msg.arg1}"
//            )
            when (msg.what) {
                MEDIA_PREPARED -> {
                    mOnPreparedListener?.onPrepared(mMediaPlayer)
                    return
                }
                MEDIA_PLAYBACK_COMPLETE -> {
                    if (mOnCompletionListener != null) {
                        mOnCompletionListener?.onCompletion(mMediaPlayer)
                    }
                    stayAwake(false)
                    return
                }
                MEDIA_BUFFERING_UPDATE -> {
                    if (mOnBufferingUpdateListener != null) {
                        mOnBufferingUpdateListener?.onBufferingUpdate(mMediaPlayer, msg.arg1)
                    }
                    return
                }
                MEDIA_SEEK_COMPLETE -> {
                    if (mOnSeekCompleteListener != null) {
                        mOnSeekCompleteListener?.onSeekComplete(mMediaPlayer)
                    }
                    return
                }
                MEDIA_SET_VIDEO_SIZE -> {
                    if (mOnVideoSizeChangedListener != null) {
                        mOnVideoSizeChangedListener?.onVideoSizeChanged(
                            mMediaPlayer,
                            msg.arg1,
                            msg.arg2
                        )
                    }
                    return
                }
                MEDIA_ERROR -> {

                    // For PV specific error values (msg.arg2) look in
                    // opencore/pvmi/pvmf/include/pvmf_return_codes.h
                    Log.e(
                        XPlayer.TAG,
                        "Error (" + msg.arg1 + "," + msg.arg2 + ")"
                    )
                    var error_was_handled = false
                    if (mOnErrorListener != null) {
                        error_was_handled =
                            mOnErrorListener?.onError(mMediaPlayer, msg.arg1, msg.arg2) ?: false
                    }
                    if (mOnCompletionListener != null && !error_was_handled) {
                        mOnCompletionListener?.onCompletion(mMediaPlayer)
                    }
                    stayAwake(false)
                    return
                }
                MEDIA_INFO -> {
                    if (msg.arg1 != MEDIA_INFO_VIDEO_TRACK_LAGGING) {
                        Log.i(
                            XPlayer.TAG,
                            "Info (" + msg.arg1 + "," + msg.arg2 + ")"
                        )
                    }
                    if (mOnInfoListener != null) {
                        mOnInfoListener?.onInfo(mMediaPlayer, msg.arg1, msg.arg2)
                    }
                    // No real default action so far.
                    return
                }
                MEDIA_TIMED_TEXT -> {

                    // do nothing
                    return
                }
                MEDIA_NOP -> {
                }
                MEDIA_CURRENT -> {
//                    Log.d(TAG, "handleMessage: MEDIA_CURRENT")
//                    if (mOnCurrentPositionListener != null) {
//                        mOnCurrentPositionListener.onCurrentPosition(
//                            msg.arg1.toLong(),
//                            msg.arg2.toLong()
//                        )
//                    }
                }
                else -> {
                    Log.e(
                        XPlayer.TAG,
                        "Unknown message type " + msg.what
                    )
                    return
                }
            }
        }


    }

    override fun getRotate(): Int {
        Log.d(TAG, "Not yet implemented")
        return 0
    }

    override fun setOnInfoListener(listener: IMediaPlayer.OnInfoListener?) {
        Log.d(TAG, "Not yet implemented")
    }

    override fun isLooping(): Boolean {
        Log.d(TAG, "Not yet implemented")
        return false
    }

    override fun getDuration(): Long {
        Log.d(TAG, "Not yet implemented")
        return 0
    }

    override fun setOnCompletionListener(listener: IMediaPlayer.OnCompletionListener?) {
        mOnCompletionListener = listener
    }

    override fun seekTo(msec: Float) {
        Log.d(TAG, "Not yet implemented")
    }

    override fun getCurrentPosition(): Long {
        Log.d(TAG, "Not yet implemented")
        return 0

    }

    override fun setAudioSessionId(sessionId: Int) {
        Log.d(TAG, "Not yet implemented")
    }

    override fun setOnPreparedListener(listener: IMediaPlayer.OnPreparedListener?) {
        Log.d(TAG, "Not yet implemented")
        mOnPreparedListener = listener
    }

    override fun setDisplay(sh: SurfaceHolder?) {
        Log.d(TAG, "Not yet implemented")
    }

    override fun setOnVideoSizeChangedListener(listener: IMediaPlayer.OnVideoSizeChangedListener?) {
        Log.d(TAG, "Not yet implemented")
        mOnVideoSizeChangedListener = listener
    }

    override fun start() {
        _start()
    }

    private external fun _start()

    override fun setVolume(leftVolume: Float, rightVolume: Float) {
        Log.d(TAG, "Not yet implemented")
    }

    override fun setDataSource(context: Context, uri: Uri) {
        setDataSourceInternal(UriUtils.uri2File(uri).absolutePath)
    }

    override fun setDataSource(context: Context, uri: Uri, headers: MutableMap<String, String>?) {
        Log.d(TAG, "Not yet implemented")
    }

    override fun setDataSource(path: String) {
        Log.d(TAG, "setDataSource mNativeContext=$mNativeContext path = $path ")
        setDataSourceInternal(path)
    }

    override fun setDataSource(path: String, headers: MutableMap<String, String>?) {
        Log.d(TAG, "Not yet implemented")
    }

    override fun setDataSource(fd: FileDescriptor?) {
        Log.d(TAG, "Not yet implemented")
    }

    override fun setDataSource(fd: FileDescriptor?, offset: Long, length: Long) {
        Log.d(TAG, "Not yet implemented")
    }

    override fun resume() {
        _resume()
    }

    private external fun _resume()

    override fun reset() {
        Log.d(TAG, "Not yet implemented")
    }

    override fun setWakeMode(context: Context?, mode: Int) {
        Log.d(TAG, "Not yet implemented")
    }

    override fun isPlaying(): Boolean {
        Log.d(TAG, "Not yet implemented")
        return true
    }

    override fun prepare() {
        Log.d(TAG, "Not yet implemented")
    }

    override fun setOnSeekCompleteListener(listener: IMediaPlayer.OnSeekCompleteListener?) {
        Log.d(TAG, "Not yet implemented")
    }

    override fun pause() {
        _pause()
    }

    private external fun _pause()

    override fun setOnErrorListener(listener: IMediaPlayer.OnErrorListener?) {
        Log.d(TAG, "Not yet implemented")
    }

    override fun prepareAsync() {
    }

    override fun setMute(mute: Boolean) {
    }

    override fun setAudioStreamType(streamtype: Int) {
    }

    override fun setPitch(pitch: Float) {
    }

    override fun getVideoWidth(): Int {
        return _getVideoWidth()
    }

    private external fun _getVideoWidth(): Int


    override fun setLooping(looping: Boolean) {
    }

    override fun setScreenOnWhilePlaying(screenOn: Boolean) {
    }

    override fun getVideoHeight(): Int {
        return _getVideoHeight()
    }

    private external fun _getVideoHeight(): Int

    override fun setSurface(surface: Surface?) {
    }

    override fun stop() {
        _stop()
    }

    private external fun _stop()

    override fun setOnBufferingUpdateListener(listener: IMediaPlayer.OnBufferingUpdateListener?) {
    }

    override fun getAudioSessionId(): Int {
        return 0
    }

    override fun release() {
        _release()
    }

    private external fun _release()

    override fun setRate(rate: Float) {
    }

}