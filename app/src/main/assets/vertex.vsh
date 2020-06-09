#version 300 es

//layout(location = 0) in vec4 vPosition;
//
//void main() {
//    gl_Position = vPosition;
//}


attribute vec4 aPosition;//顶点坐标
attribute vec2 aTexCoord;//材质顶点坐标
varying vec2 vTexCoord;//输出的才制作表
void main() {
    vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
    gl_Position = aPosition;
}
