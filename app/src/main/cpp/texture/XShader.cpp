//
// Created by 王越 on 2020/4/30.
//

#include "XShader.h"
#include <GLES2/gl2.h>
#include <jni.h>
#include <unistd.h>

#include "../utils/OpenGLUtils.h"
#include "../XLog.h"
#include <EGL/egl.h>
#include <cstdlib>



//顶点着色器glsl
#define GET_STR(x) #x
char vertexShader[] =
        "#version 300 es\n"
        "layout(location = 0) in vec4 aPosition;//顶点坐标\n"
        "layout(location = 1) in vec2 aTexCoord;//材质顶点坐标\n"
        "out vec2 vTexCoord;//输出的才制作表\n"
        "void main() {\n"
        "    vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);\n"
        "    gl_Position = aPosition;\n"
        "}";

//片元着色器,软解码和部分x86硬解码
char fragYUV420P[] =
        "#version 300 es\n"
        "precision mediump float;//精度\n"
        "in vec2 vTexCoord; //顶点着色器传递的坐标\n"
        "out vec4 gl_FragColor;\n"
        "//输入的材质(不透明灰度图，单像素)\n"
        "uniform sampler2D yTexture;\n"
        "uniform sampler2D uTexture;\n"
        "uniform sampler2D vTexture;\n"
        "\n"
        "void main() {\n"
        "    vec3 yuv;\n"
        "    vec3 rgb;\n"
        "    yuv.r = texture(yTexture, vTexCoord).r - (16.0 / 255.0);   \n"
        "    yuv.g = texture(uTexture, vTexCoord).r - 0.5;              \n"
        "    yuv.b = texture(vTexture, vTexCoord).r - 0.5;              \n"
        "                                                               \n"
        "    rgb = mat3(1.164,  1.164,  1.164,                   \n"
        "               0.0,   -0.213,  2.112,                       \n"
        "               1.793, -0.533,    0.0) * yuv;                   \n"
        "    //输出像素颜色\n"
        "    gl_FragColor = vec4(rgb, 1.0);\n"
        "\n"
        "}";


bool XShader::init() {
    program = OpenGLUtils::createProgram(vertexShader, fragYUV420P);
    OpenGLUtils::checkGLError("createProgram");
    GLint aPosition = glGetAttribLocation(program, "aPosition");
    GLint aTexCoord = glGetAttribLocation(program, "aTexCoord");
    inputTextureHandle[0] = glGetUniformLocation(program, "yTexture");
    inputTextureHandle[1] = glGetUniformLocation(program, "uTexture");
    inputTextureHandle[2] = glGetUniformLocation(program, "vTexture");
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glUseProgram(program);

    ////////////////////////
//    GLfloat vertices[] = { //正方形
//            1.0f, -1.0f, 0.0f, // 右下
//            1.0f, 0.0f,   // 纹理右下
//            -1.0f, -1.0f, 0.0f, // 左下
//            0.0f, 0.0f,//纹理左下
//            -1.0f, 1.0f, 0.0f, // 左上
//            0.0f, 1.0f,// 纹理左上
//            1.0f, 1.0f, 0.0f,  // 右上
//            1.0f, 1.0f // 纹理右上
//    };
//    GLubyte indices[] = {
//            0, 1, 2, 0, 2, 3
//    };
//    GLuint vboIds[2];
//    glGenBuffers(2, vboIds);
//
//    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);///顶点缓冲
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_DYNAMIC_DRAW);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);///索引缓冲
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);
//
//    GLuint vaoId;
//    glen




    ////////////////////////
    //加入三维顶点数据 ，两个三角形组成正方形
    static float ver[] = {
            -1.0f, -1.0f,  // left,  bottom
            1.0f, -1.0f,  // right, bottom
            -1.0f, 1.0f,  // left,  top
            1.0f, 1.0f,  // right, top
    };
    glEnableVertexAttribArray(aPosition);//确定是否有效
    glVertexAttribPointer(
            aPosition, //顶点属性索引
            2,         //分量的数量
            GL_FLOAT,
            GL_FALSE,
            2 * sizeof(float),         //指定顶点索引 i和i+1 之间的位移，0代表每个顶点属性数据顺序存储，大于0表示获取下一个索引的跨距
            ver
    );
    static float texture[] = {
            0.0f, 1.0f, // left, top
            1.0f, 1.0f, // right, top
            0.0f, 0.0f, // left, bottom
            1.0f, 0.0f, // right, bottom
    };
    glEnableVertexAttribArray(aTexCoord);//确定是否有效
    glVertexAttribPointer(
            aTexCoord,
            2,
            GL_FLOAT,
            GL_FALSE,
            2 * sizeof(float),
            texture);

    LOGI("egl 初始化shader  success !");
    glClear(GL_COLOR_BUFFER_BIT);

    if (textures[0] == 0) {
        glGenTextures(3, textures);
    }
    for (int i = 0; i < 3; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glUniform1i(inputTextureHandle[i], i);
    }

    return true;
}

void XShader::draw() {
    if (!program)return;
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

void XShader::getTexture(unsigned int i, int width, int height, unsigned char *buf) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, textures[i]);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_LUMINANCE,
                 width,
                 height,
                 0,
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 buf);
    glUniform1i(inputTextureHandle[i], i);
}
