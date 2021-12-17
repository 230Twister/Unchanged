#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;  // �л�����ռ�ľ���
uniform mat4 model;             // ����ռ�ת������

void main() {
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
}