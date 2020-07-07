package com.joeys.xplay

import android.app.Activity
import android.content.Intent
import android.net.Uri
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import com.blankj.utilcode.util.ScreenUtils
import com.blankj.utilcode.util.UriUtils
import com.joeys.xplay.activity.PlayerActivity
import com.joeys.xplay.metadata.MediaMetadataRetriever
import com.joeys.xplay.metadata.XMetadata
import com.yanzhenjie.permission.AndPermission
import com.yanzhenjie.permission.runtime.Permission
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        AndPermission.with(this)
            .runtime()
            .permission(Permission.Group.STORAGE)
            .onGranted { permissions: List<String?>? ->
            }
            .onDenied { permissions: List<String?>? ->

            }
            .start()
        btn.setOnClickListener {
            val file = File(
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
                "失落沙洲.mp4"
            )
            open(file.absolutePath)
        }
        test.setOnClickListener {
            val intent= Intent(Intent.ACTION_VIEW)
            intent.setData(Uri.parse("http://weixin.qq.com/r/o3W_sRvEMSVOhwrSnyCH"))
            intent.setPackage("com.tencent.mm")
            intent.putExtra(Intent.EXTRA_SUBJECT,"Share")
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            startActivity(intent)
        }
        btn_scan.setOnClickListener {

            val intent = Intent();
            intent.action = Intent.ACTION_GET_CONTENT
            intent.type = "video/*"
            startActivityForResult(intent, 111)
        }
    }

    val TAG = "xplay"
    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (resultCode == Activity.RESULT_OK) {
            data?.let {
                val uri = it.data
                Log.d(TAG, "onActivityResult: $uri")
                if (uri != null) {
                    val uri2File = UriUtils.uri2File(uri)
                    Log.d(TAG, "uri2File: ${uri2File.absolutePath}")
//                    open(uri2File.absolutePath)
                    PlayerActivity.start(uri2File.absolutePath, this)
                }
            }
        }
    }


    private fun open(path: String) {
//        val metadataRetriever = MediaMetadataRetriever()
//        metadataRetriever.setDataSource(path)
//        metadataRetriever.getMetadata()?.let {
//            val width = it.getInt("video_width")
//            val height = it.getInt("video_height")
//            val layoutParams = xplay.layoutParams
//            layoutParams.width = ScreenUtils.getScreenWidth()
//            layoutParams.height = ScreenUtils.getScreenWidth() * height / width
//            xplay.layoutParams = layoutParams
//
//        }
//        xplay.open(path)

    }


}
