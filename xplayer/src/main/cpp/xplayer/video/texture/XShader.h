//
// Created by 王越 on 2020/8/9.
//

#ifndef XPLAY_XSHADER_H
#define XPLAY_XSHADER_H
#define MAX_TEXTURES 32

#include <GLES3/gl3.h>

enum XShaderType {
    XSHADER_YUV420P = 0,
    XSHADER_NV12 = 25,
    XSHADER_NV21 = 26,
};

class XShader {
public:
    XShader();

    bool init(XShaderType shaderType = XSHADER_YUV420P);

    //获取材质并映射到内存
    void getTexture(unsigned int index, int width, int height, unsigned char *buf);

    void draw();

    virtual ~XShader();

    int inputTextureHandle[MAX_TEXTURES]; // 纹理句柄列表

    GLfloat vertices[20];
protected:
    unsigned int vertexSH = 0;
    unsigned int fragSH = 0;
    unsigned int program = 0;
    unsigned int textures[100] = {0};
public:
    float matrix[16];

};


#endif //XPLAY_XSHADER_H
