#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Model/Moon.h"

Moon::Moon(glm::vec4 c, float r, float t)
{
    memset(vertices, 0, sizeof(vertices));
    moonLightDirection = glm::vec3(-1.0f, 0.0f, 0.0f);
    color = c;
    radius = r;
    trackRadius = t;

    // 构造顶点数组
    float lon_step = 1.0f / lons;
    float lat_step = 1.0f / lats;
    GLuint offset = 0;
    for (int lat = 0; lat < lats; lat++) {  // 纬线u
        for (int lon = 0; lon < lons; lon++) { // 经线v
            // 一次构造4个点，两个三角形
            glm::vec3 point1 = GetPoint(lat * lat_step, lon * lon_step);
            glm::vec3 point2 = GetPoint((lat + 1) * lat_step, lon * lon_step);
            glm::vec3 point3 = GetPoint((lat + 1) * lat_step, (lon + 1) * lon_step);
            glm::vec3 point4 = GetPoint(lat * lat_step, (lon + 1) * lon_step);
            memcpy(vertices + offset, value_ptr(point1), 3 * sizeof(float));
            offset += 3;
            memcpy(vertices + offset, value_ptr(color), 4 * sizeof(float));
            offset += 4;
            memcpy(vertices + offset, value_ptr(point4), 3 * sizeof(float));
            offset += 3;
            memcpy(vertices + offset, value_ptr(color), 4 * sizeof(float));
            offset += 4;
            memcpy(vertices + offset, value_ptr(point3), 3 * sizeof(float));
            offset += 3;
            memcpy(vertices + offset, value_ptr(color), 4 * sizeof(float));
            offset += 4;

            memcpy(vertices + offset, value_ptr(point1), 3 * sizeof(float));
            offset += 3;
            memcpy(vertices + offset, value_ptr(color), 4 * sizeof(float));
            offset += 4;
            memcpy(vertices + offset, value_ptr(point3), 3 * sizeof(float));
            offset += 3;
            memcpy(vertices + offset, value_ptr(color), 4 * sizeof(float));
            offset += 4;
            memcpy(vertices + offset, value_ptr(point2), 3 * sizeof(float));
            offset += 3;
            memcpy(vertices + offset, value_ptr(color), 4 * sizeof(float));
            offset += 4;
        }
    }
    moonShader = new Shader("../../../shader/SMVert.vs", "../../../shader/SMFrag.frag");
    LoadMoon();
}

glm::vec3 Moon::GetPoint(float u, float v)
{
    constexpr float _pi = glm::pi<float>();
    float z = radius * cos(_pi * u);
    float x = radius * sin(_pi * u) * cos(2 * _pi * v);
    float y = radius * sin(_pi * u) * sin(2 * _pi * v);
    return glm::vec3(x, y, z);
}

void Moon::Render(float a)
{
    glBindVertexArray(moonVAO);
    // 计算月亮位置并渲染
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(trackRadius * cos(a), trackRadius * sin(a), 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(moonShader->getID(), "model"), 1, GL_FALSE, value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 6 * Moon::lats * Moon::lons);
    glBindVertexArray(0);
    // 修改光线方向
    moonLightDirection.x = cos(a);
    moonLightDirection.y = sin(a);
}

glm::vec3 Moon::GetLightDirection()
{
    return moonLightDirection;
}

void Moon::LoadMoon()
{
    glGenVertexArrays(1, &moonVAO);
    glGenBuffers(1, &moonVBO);

    glBindVertexArray(moonVAO);
    glBindBuffer(GL_ARRAY_BUFFER, moonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
