#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;                // ������
out vec3 FragPos;               // Ƭ������
out vec2 TexCoords;             // ��������
out vec4 FragLightSpacePos;     // Ƭ��λ�ڹ�ռ������

uniform mat4 model;                 // ����ռ�ת������
uniform mat4 view;                  // �ӽǿռ�ת������
uniform mat4 projection;            // ͶӰ�ռ�ת������
uniform mat4 lightSpaceMatrix;      // ��ռ�ת������

void main()
{
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0f));
    FragLightSpacePos = lightSpaceMatrix * vec4(FragPos, 1.0f);
    Normal = aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}