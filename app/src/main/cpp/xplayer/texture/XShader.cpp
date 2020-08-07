//
// Created by 王越 on 2020/4/30.
//

#include "XShader.h"
#include <GLES3/gl3.h>
#include <jni.h>
#include <unistd.h>

#include "../utils/OpenGLUtils.h"
#include "../../XLog.h"
#include "../utils/CoordinateUtils.h"
#include <EGL/egl.h>
#include <cstdlib>


//顶点着色器glsl
#define GET_STR(x) #x
char vertexShader[] =
        "#version 300 es\n"
        "layout(location = 0) in vec4 aPosition;//顶点坐标\n"
        "layout(location = 1) in vec2 aTexCoord;//材质顶点坐标\n"
        "//uniform mat4 uMVPMatrix;\n"
        "out vec2 vTexCoord;//输出的才制作表\n"
        "void main() {\n"
        "    vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);\n"
        "    gl_Position = aPosition ;\n"
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

XShader::~XShader() {
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
}

XShader::XShader() {
    auto coor = const_cast<GLfloat *>(CoordinateUtils::getInputTextureCoordinates());
    for (int i = 0; i < 20; ++i) {
        vertices[i] = coor[i];
    }
}

bool XShader::init(XShaderType shaderType) {
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
    GLubyte indices[] = {
            0, 1, 2, 0, 2, 3
    };
    ///正确的绑定顺序是VAO、VBO、EBO,将后两者的信息也绑定进VAO中
    GLuint vboIds[2];
    glGenBuffers(2, vboIds);


    ///把顶点数组复制到顶点缓冲中
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);///顶点缓冲
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_DYNAMIC_DRAW);

    ///把索引数组复制到顶点缓冲中
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);///索引缓冲
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);

    GLuint vaoId;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);

    glEnableVertexAttribArray(aPosition);
    glEnableVertexAttribArray(aTexCoord);

    GLint vertexSize = 3;
    GLint textureCount = 2;

    glVertexAttribPointer(aPosition,
                          vertexSize,
                          GL_FLOAT, GL_FALSE,
                          (vertexSize + textureCount) * sizeof(float),
                          0 ///使用顶点缓冲对象
    );

    glVertexAttribPointer(aTexCoord,
                          textureCount,
                          GL_FLOAT, GL_FALSE,
                          (vertexSize + textureCount) * sizeof(float),
                          (const void *) ((vertexSize) * sizeof(float))///偏移量
    );
    ////////////////////////

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
    glBindVertexArray(vaoId);
    LOGI("egl 初始化shader  success !");

    return true;
}

void XShader::draw() {
    if (!program)return;
    glDrawElements(GL_TRIANGLES,
                   6,
                   GL_UNSIGNED_BYTE,
                   0
    );
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
