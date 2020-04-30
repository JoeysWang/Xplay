//
// Created by 王越 on 2020/4/30.
//

#include "XShader.h"
#include <GLES2/gl2.h>
#include <jni.h>
#include <unistd.h>

#include "../XLog.h"
#include <EGL/egl.h>



//顶点着色器glsl
#define GET_STR(x) #x
static const char *vertexShader = GET_STR(
        attribute
        vec4 aPosition; //顶点坐标
        attribute
        vec2 aTexCoord; //材质顶点坐标
        varying
        vec2 vTexCoord; //输出的才制作表
        void main() {
            vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
            gl_Position = aPosition;
        }
);

//片元着色器,软解码和部分x86硬解码
static const char *fragYUV420P = GET_STR(
        precision
        mediump float;//精度
        varying
        vec2 vTexCoord; //顶点着色器传递的坐标
        //输入的材质(不透明灰度图，单像素)
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;

        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(yTexture, vTexCoord).r;
            yuv.g = texture2D(uTexture, vTexCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTexCoord).r - 0.5;

            rgb = mat3(1.0, 1.0, 1.0,
                       0.0, -0.39465, 2.03211,
                       1.13983, -0.5806, 0.0) * yuv;
            //输出像素颜色
            gl_FragColor = vec4(rgb, 1.0);

        }

);

static GLuint initShader(const char *code, GLint type) {
    //创建shader
    GLuint sh = glCreateShader(type);
    if (sh == 0) {
        LOGE("initShader failed %d", type);
        return 0;
    }
    //加载shader
    glShaderSource(sh,
                   1,//shader数量
                   &code, //shader代码
                   0 //代码长度
    );
    //编译shader
    glCompileShader(sh);
    //获取编译情况
    GLint glStatus;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &glStatus);
    if (glStatus == 0) {
        LOGE("gl compile failed %d", glStatus);
        return 0;
    }
    LOGI("gl compile success ");
    return sh;
}

bool XShader::init() {

    //shader 初始化
    vertexSH = initShader(vertexShader, GL_VERTEX_SHADER);
    fragSH = initShader(fragYUV420P, GL_FRAGMENT_SHADER);

    if (vertexSH == 0 || fragSH == 0) {
        LOGE("egl initShader failed !");
        return false;
    }

    //创建渲染程序
    program = glCreateProgram();
    //向程序加入着色器代码
    glAttachShader(program, vertexSH);
    glAttachShader(program, fragSH);
    //链接程序
    glLinkProgram(program);
    GLint progremStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &progremStatus);
    if (progremStatus != GL_TRUE) {
        LOGE("egl progrem link  failed !");
        return false;
    }
    LOGI("egl progrem link  success !");

    ////////////////////////
    //加入三维顶点数据 ，两个三角形组成正方形
    static float ver[] = {
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
    };
    GLint aPosition = glGetAttribLocation(program, "aPosition");
    glEnableVertexAttribArray(aPosition);//确定是否有效
    //传递顶点值
    glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 12, ver);
    //加入材质坐标数据
    static float texture[] = {
            1.0f, 0.0f,//右下
            0.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
    };
    GLint aTexCoord = glGetAttribLocation(program, "aTexCoord");
    glEnableVertexAttribArray(aTexCoord);//确定是否有效
    glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 8, texture);

    glUniform1i(glGetUniformLocation(program, "yTexture"), 0);
    glUniform1i(glGetUniformLocation(program, "uTexture"), 1);
    glUniform1i(glGetUniformLocation(program, "vTexture"), 2);

    LOGI("egl 初始化shader  success !");

    return true;
}

void XShader::draw() {
    if (!program)return;
    LOGD("xshader draw ");
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void XShader::getTexture(unsigned int index, int width, int height, unsigned char *buf) {
//    LOGD("getTexture index=%d,  width=%d,  height=%d , bufSize =%d", index, width, height, sizeof(buf));
    if (textures[index] == 0) {
        //材质初始化
        glGenTextures(1, &textures[index]);
        glBindTexture(GL_TEXTURE_2D, textures[index]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_LUMINANCE,
                     width, height,
                     0,
                     GL_LUMINANCE,
                     GL_UNSIGNED_BYTE,
                     NULL
        );
    }

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, textures[index]);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0, 0, 0,
                    width, height,
                    GL_LUMINANCE,
                    GL_UNSIGNED_BYTE,
                    buf
    );
//    LOGD("bind success index=%d,  width=%d,  height=%d , bufSize =%d", index, width, height, sizeof(buf));

}
