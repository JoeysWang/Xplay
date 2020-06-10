//
// Created by 王越 on 2020/4/30.
//

#ifndef XPLAY_XSHADER_H
#define XPLAY_XSHADER_H
#define MAX_TEXTURES 32


class XShader {
public:
    virtual bool init();

    //获取材质并映射到内存
    void getTexture(unsigned int index, int width, int height, unsigned char *buf);

    void draw();

    int inputTextureHandle[MAX_TEXTURES]; // 纹理句柄列表


protected:
    unsigned int vertexSH = 0;
    unsigned int fragSH = 0;
    unsigned int program = 0;
    unsigned int textures[100] = {0};
public:
    float matrix[16];

};


#endif //XPLAY_XSHADER_H
