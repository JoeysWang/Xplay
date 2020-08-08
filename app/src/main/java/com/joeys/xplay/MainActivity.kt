package com.joeys.xplay

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.os.Environment
import android.os.Handler
import android.os.Looper
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.blankj.utilcode.util.UriUtils
import com.joeys.xplay.activity.PlayerActivity
import com.sample.breakpad.BreakpadInit
import com.yanzhenjie.permission.AndPermission
import com.yanzhenjie.permission.runtime.Permission
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File

class MainActivity : AppCompatActivity() {
    private var externalReportPath: File? = null
    val TAG = "xplay"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        AndPermission.with(this)
            .runtime()
            .permission(Permission.Group.STORAGE)
            .onGranted { permissions: List<String?>? ->
                initExternalReportPath()
//                initBreakPad()
            }
            .onDenied { permissions: List<String?>? ->

            }
            .start()

        btn.setOnClickListener {
            val file = File(
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
                "失落沙洲.mp4"
            )
            player.setDataSource(file.absolutePath)
        }

        test.setOnClickListener {

        }
        btn_scan.setOnClickListener {

            val intent = Intent();
            intent.action = Intent.ACTION_GET_CONTENT
            intent.type = "video/*"
            startActivityForResult(intent, 111)
        }
    }

    private fun initExternalReportPath() {
        externalReportPath =
            File(Environment.getExternalStorageDirectory(), "crashDump")
        if (externalReportPath?.exists() != true) {
            externalReportPath?.mkdirs()
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (resultCode == Activity.RESULT_OK) {
            data?.let {
                val uri = it.data
                Log.d(TAG, "onActivityResult: $uri")
                if (uri != null) {
                    val uri2File = UriUtils.uri2File(uri)
                    Log.d(TAG, "uri2File: ${uri2File.absolutePath}")
//                    PlayerActivity.start(uri2File.absolutePath, this)
                }
            }
        }
    }

    private fun initBreakPad() {
        if (externalReportPath == null) {
            externalReportPath = File(filesDir, "crashDump")
            if (!externalReportPath!!.exists()) {
                externalReportPath!!.mkdirs()
            }
        }
        BreakpadInit.initBreakpad(externalReportPath!!.absolutePath)
    }


}
