package com.joeys.xplay.activity

import android.content.Context
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import com.blankj.utilcode.util.ScreenUtils
import com.joeys.xplay.R
import com.joeys.xplay.metadata.MediaMetadataRetriever
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.activity_player.*

class PlayerActivity : AppCompatActivity() {

    private val url by lazy { intent.getStringExtra("url") ?: "" }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_player)
        btn_start_pause.setOnClickListener {
            open()
        }
    }


    private val mediaMetadataRetriever by lazy {  MediaMetadataRetriever()}

    fun open() {
        val retriever = mediaMetadataRetriever
        retriever.setDataSource(url)
        retriever.getMetadata()?.let {
            Log.d("xplay", "Metadata: $it")
            val width = it.getInt("video_width")
            val height = it.getInt("video_height")
            val layoutParams = xplay.layoutParams
            layoutParams.width = ScreenUtils.getScreenWidth()
            layoutParams.height = ScreenUtils.getScreenWidth() * height / width
            xplay.layoutParams = layoutParams
        }
        xplay.open(url)

    }

    override fun onResume() {
        super.onResume()
        xplay.resume()
    }

    override fun onPause() {
        super.onPause()
        xplay.pause()
    }

    override fun onStop() {
        super.onStop()
        xplay.stop()
    }

    override fun onDestroy() {
        super.onDestroy()
        xplay.stop()
        xplay.release()
        mediaMetadataRetriever.release()
    }

    companion object {
        fun start(url: String, context: Context) {
            val intent = Intent(context, PlayerActivity::class.java)
            intent.putExtra("url", url)
            context.startActivity(intent)
        }
    }
}