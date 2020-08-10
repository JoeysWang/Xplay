//
// Created by 王越 on 2020/8/9.
//

#ifndef XPLAY_XTEXTURE_H
#define XPLAY_XTEXTURE_H

#include <thread>
#include <memory>
#include "XShader.h"
#include "XEGL.h"

enum XTextureType {
    XTEXTURE_YUV420P = 0,
    XTEXTURE_NV12 = 25,
    XTEXTURE_NV21 = 26,
};

class XTexture {
public:

    XTexture(const std::shared_ptr<XEGL> &xegl);

    virtual bool init(
            void *win,
            XTextureType textureType = XTEXTURE_YUV420P
    );

    virtual void draw(unsigned char *data[], int width[], int height);

    void setFrameWidthHeight(int width, int height) {
        frameHeight = height;
        frameWidth = width;
    };

public:
    int frameWidth;//帧宽度
    int frameHeight;
    float lastNormalized = 0.0f;//上次裁剪

protected:
    std::unique_ptr<XShader> shader;
    std::shared_ptr<XEGL> xegl;
};


#endif //XPLAY_XTEXTURE_H
