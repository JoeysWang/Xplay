//
// Created by 王越 on 2020/4/30.
//

#ifndef XPLAY_XTEXTURE_H
#define XPLAY_XTEXTURE_H

enum XTextureType {
    XTEXTURE_YUV420P = 0,
    XTEXTURE_NV12 = 25,
    XTEXTURE_NV21 = 26,
};

class XTexture {
public:
    virtual ~XTexture() {};

    static XTexture *create();

    virtual bool init(
            void *win,
            int width,
            int height,
            XTextureType textureType = XTEXTURE_YUV420P
    ) = 0;

    virtual void draw(unsigned char *data[], int width[], int height) = 0;

public:
    int frameWidth;//帧宽度
    int frameHeight;
    float lastNormalized = 0.0f;//上次裁剪

};


#endif //XPLAY_XTEXTURE_H
