//
// Created by 王越 on 2020/7/21.
//

#include <GLES3/gl3.h>
#include "CoordinateUtils.h"

static const GLfloat vertices_default[] = {
        1.0f, -1.0f, 0.0f, // 右下
        1.0f, 0.0f,   // 纹理右下
        -1.0f, -1.0f, 0.0f, // 左下
        0.0f, 0.0f,//纹理左下
        -1.0f, 1.0f, 0.0f, // 左上
        0.0f, 1.0f,// 纹理左上
        1.0f, 1.0f, 0.0f,  // 右上
        1.0f, 1.0f // 纹理右上
};

const GLfloat *CoordinateUtils::getInputTextureCoordinates() {
    return vertices_default;
}
