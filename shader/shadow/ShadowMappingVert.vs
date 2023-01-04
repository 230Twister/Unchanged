#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;  // 切换到光空间的矩阵
uniform mat4 model;             // 世界空间转换矩阵

void main() {
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
}