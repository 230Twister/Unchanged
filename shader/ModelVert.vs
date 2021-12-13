#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;                // ������
out vec3 FragPos;               // Ƭ������
out vec2 TexCoords;             // ��������
out vec4 FragDirectSpacePos;    // Ƭ��λ��ƽ�й�ռ������
out vec4 FragSpotSpacePos;      // Ƭ��λ�ھ۹�ռ������

uniform mat4 model;                 // ����ռ�ת������
uniform mat4 view;                  // �ӽǿռ�ת������
uniform mat4 projection;            // ͶӰ�ռ�ת������
uniform mat4 directSpaceMatrix;     // ƽ�й�ռ�ת������
uniform mat4 spotSpaceMatrix;       // �۹�ռ�ת������

void main()
{
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0f));
    FragDirectSpacePos = directSpaceMatrix * vec4(FragPos, 1.0f);
    FragSpotSpacePos = spotSpaceMatrix * vec4(FragPos, 1.0f);
    Normal = aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}