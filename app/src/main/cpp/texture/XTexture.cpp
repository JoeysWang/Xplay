//
// Created by 王越 on 2020/4/30.
//

#include "XTexture.h"
#include "GLES2/gl2.h"
#include "XEGL.h"
#include "XShader.h"

class CXTexture : public XTexture {

public:
    XShader shader;

    bool init(void *win) override {
        if (!win)return false;
        XEGL *xegl = XEGL::get();
        if (!xegl) return false;
        xegl->init(win);
        shader.init();
        return true;
    }

    void draw(unsigned char *data[], int width[], int height) override {
        shader.getTexture(0, width[0], height, data[0]); //y
        shader.getTexture(1, width[1], height / 2, data[1]); //u
        shader.getTexture(2, width[2], height / 2, data[2]); //v
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        shader.draw();
    }
};

XTexture *XTexture::create() {
    return new CXTexture();
}
