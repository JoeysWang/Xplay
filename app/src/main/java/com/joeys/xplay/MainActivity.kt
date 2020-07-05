package com.joeys.xplay

import android.app.Activity
import android.app.Dialog
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.os.Looper
import android.util.Log
import android.view.ViewManager
import com.blankj.utilcode.util.FileUtils
import com.blankj.utilcode.util.ScreenUtils
import com.blankj.utilcode.util.UriUtils
import com.yanzhenjie.permission.AndPermission
import com.yanzhenjie.permission.runtime.Permission
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File
import java.util.concurrent.locks.ReentrantLock

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
            open()
        }
        test.setOnClickListener {
            xplay.text()
        }
        btn_scan.setOnClickListener {

            val intent = Intent();
            intent.action = Intent.ACTION_GET_CONTENT
            intent.type = "video/*"
            startActivityForResult(intent, 111)
        }

        xplay.layoutParams.height = 1080 * ScreenUtils.getScreenWidth() / 1920
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
                    xplay.open(uri2File.absolutePath)
                }
            }
        }
    }

    private fun open() {
        val file = File(
            Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
            "失落沙洲.mp4"
        )
        xplay.open(file.absolutePath)
    }


}
