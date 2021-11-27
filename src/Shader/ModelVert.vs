#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;                // 法向量
out vec3 FragPos;               // 片段坐标
out vec2 TexCoords;             // 纹理坐标
out vec4 FragLightSpacePos;     // 片段位于光空间的坐标

uniform mat4 model;                 // 世界空间转换矩阵
uniform mat4 view;                  // 视角空间转换矩阵
uniform mat4 projection;            // 投影空间转换矩阵
uniform mat4 lightSpaceMatrix;      // 光空间转换矩阵

void main()
{
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0f));
    FragLightSpacePos = lightSpaceMatrix * vec4(FragPos, 1.0f);
    Normal = aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}