package com.joeys.xplay

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
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
                open()
            }
            .onDenied { permissions: List<String?>? ->

            }
            .start()
    }

    private fun open() {
        val file = File(
            Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
            "失落沙洲.mp4"
        )
       Xplay.open(file.absolutePath)

    }



}
