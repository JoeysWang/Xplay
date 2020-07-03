package com.joeys.xplay

import android.app.Dialog
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.os.Looper
import android.view.ViewManager
import com.blankj.utilcode.util.ScreenUtils
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

        xplay.layoutParams.height = 1080 * ScreenUtils.getScreenWidth() / 1920
    }

    private fun open() {
        val file = File(
            Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
            "失落沙洲.mp4"
        )
        xplay.open(file.absolutePath)
    }


}
