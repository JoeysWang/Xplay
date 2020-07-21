//
// Created by 王越 on 2020/4/30.
//

#include "XTexture.h"
#include "GLES2/gl2.h"
#include "XEGL.h"
#include "XShader.h"
#include "../../XLog.h"
#include "../utils/vecmath.h"

class CXTexture : public XTexture {

public:
    XShader shader;

    bool init(void *win, int width, int height, XTextureType textureType) override {
        if (!win)return false;
        XEGL *xegl = XEGL::get();
        if (!xegl) return false;
        xegl->init(win, width, height);
        frameWidth = width;
        frameHeight = height;
        shader.init();
        return true;
    }

    void draw(unsigned char *data[], int width[], int height) override {
        if (width[0] != frameWidth) {
            ///对帧宽度和纹理宽度不一致的进行裁剪
            GLsizei padding = width[0] - frameWidth;
            GLfloat normalized = ((GLfloat) padding + 0.5f) / (GLfloat) width[0];
            LOGI("帧宽度和linesize宽度不一致 padding=%d , normalized=%f", padding, normalized);
        }

        shader.getTexture(0, width[0], height, data[0]); //y
        shader.getTexture(1, width[1], height / 2, data[1]); //u
        shader.getTexture(2, width[2], height / 2, data[2]); //v
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        shader.draw();
    }

    ~CXTexture() {}
};

XTexture *XTexture::create() {
    return new CXTexture();
}

