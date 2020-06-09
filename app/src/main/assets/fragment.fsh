#version 300 es

//precision mediump float;
//out vec4 fragColor;
//
//void main() {
//    fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );
//}

precision mediump float;//精度
varying vec2 vTexCoord; //顶点着色器传递的坐标
//输入的材质(不透明灰度图，单像素)
uniform sampler2D yTexture;
uniform sampler2D uTexture;
uniform sampler2D vTexture;

void main() {
    vec3 yuv;
    vec3 rgb;
    yuv.r = texture2D(yTexture, vTexCoord).r;
    yuv.g = texture2D(uTexture, vTexCoord).r - 0.5;
    yuv.b = texture2D(vTexture, vTexCoord).r - 0.5;

    rgb = mat3(1.0, 1.0, 1.0,
    0.0, -0.39465, 2.03211,
    1.13983, -0.5806, 0.0) * yuv;
    //输出像素颜色
    gl_FragColor = vec4(rgb, 1.0);

}