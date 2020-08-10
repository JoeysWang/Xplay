//
// Created by 王越 on 2020/8/9.
//

#include <XLog.h>
#include "XTexture.h"
#include "../../utils/CoordinateUtils.h"
#include <GLES/gl.h>

XTexture::XTexture(const std::shared_ptr<XEGL> &xegl) : xegl(xegl) {
    LOGI("XTexture::XTexture");

}

bool XTexture::init(void *win, XTextureType textureType) {
    if (!win)return false;
    if (!xegl) return false;
    xegl->init(win);
    shader = std::make_unique<XShader>();
    shader->init();
    return true;
}

void XTexture::draw(unsigned char **data, int *width, int height) {
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
