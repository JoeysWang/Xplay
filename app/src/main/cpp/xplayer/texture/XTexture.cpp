//
// Created by 王越 on 2020/4/30.
//

#include "XTexture.h"
#include "GLES2/gl2.h"
#include "XEGL.h"
#include "../../XLog.h"
#include "../utils/vecmath.h"
#include "../utils/CoordinateUtils.h"
#include "../../xhandler/ThreadUtils.h"

class CXTexture : public XTexture {

public:

    bool init(void *win, XTextureType textureType) override {
        LOGI("CXTexture::init thread id=%ld", ThreadUtils::currentId());
        XEGL::get()->clear();
        if (!win)return false;
        XEGL *xegl = XEGL::get();
        LOGI("CXTexture::init xegl =%p",xegl);
        if (!xegl) return false;
        xegl->init(win);
        shader = new XShader;
        shader->init();
        return true;
    }


    void draw(unsigned char *data[], int width[], int height) override {
        if (width[0] != frameWidth) {
            ///对帧宽度和纹理宽度不一致的进行裁剪
            GLsizei padding = width[0] - frameWidth;
            GLfloat normalized = ((GLfloat) padding + 0.5f) / (GLfloat) width[0];
            if (lastNormalized != normalized) {
                auto coor = const_cast<GLfloat *>(CoordinateUtils::getInputTextureCoordinates());
                for (int i = 0; i < 20; ++i) {
                    if (i == 3 || i == 18) {
                        shader->vertices[i] = coor[i] - normalized;
                        LOGI(" shader.vertices[%d] = %f", i, shader->vertices[i]);
                    } else
                        shader->vertices[i] = coor[i];
                }
                lastNormalized = normalized;
                shader->init();
            }
        }

        shader->getTexture(0, width[0], height, data[0]); //y
        shader->getTexture(1, width[1], height / 2, data[1]); //u
        shader->getTexture(2, width[2], height / 2, data[2]); //v
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        shader->draw();
    }

    ~CXTexture() {
        delete shader;
        shader = nullptr;
    }
};

XTexture *XTexture::create() {
    return new CXTexture();
}

