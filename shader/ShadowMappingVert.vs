#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;  // �л�����ռ�ľ���

void main() {
    gl_Position = lightSpaceMatrix * vec4(position, 1.0f);
}