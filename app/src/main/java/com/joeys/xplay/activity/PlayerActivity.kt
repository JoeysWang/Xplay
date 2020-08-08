package com.joeys.xplay.activity

import android.content.Context
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import com.blankj.utilcode.util.ScreenUtils
import com.joeys.xplay.R
import com.joeys.xplayer.metadata.MediaMetadataRetriever
import kotlinx.android.synthetic.main.activity_player.*

class PlayerActivity : AppCompatActivity() {

    private val url by lazy { intent.getStringExtra("url") ?: "" }
//    private val retriever = MediaMetadataRetriever()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_player)
        btn_start.setOnClickListener {
            xplay.start()
        }
        btn_pause.setOnClickListener {
            xplay.pause()
        }
        btn_resume.setOnClickListener {
            xplay.resume()
        }
        xplay.setDataSource(url)
    }

    fun setDataSource() {
//        retriever.setDataSource(url)
//        retriever.getMetadata()?.let {
//            xplay.setDataSource(url)
//            Log.d("xplay", "Metadata: $it")
//            val width = xplay.videoWidth
//            val height = xplay.videoHeight
//            Log.d("xplay", "width: $width  height:$height")
//            val layoutParams = xplay.layoutParams
//            layoutParams.width = ScreenUtils.getScreenWidth()
//            layoutParams.height = ScreenUtils.getScreenWidth() * height / width
//            xplay.layoutParams = layoutParams
//            info.text = it.toString()
//        }
//        retriever.release()
    }

    companion object {
        fun start(url: String, context: Context) {
            val intent = Intent(context, PlayerActivity::class.java)
            intent.putExtra("url", url)
            context.startActivity(intent)
        }
    }
}