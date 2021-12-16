#include "Model/Star.h"

Star::Star(glm::vec4 c, GLfloat r, GLfloat t)
{
    memset(vertices, 0, sizeof(vertices));
    lightDirection = glm::vec3(-1.0f, 0.0f, 0.0f);
    color = c;
    radius = r;
    trackRadius = t;

    // 构造顶点数组
    GLfloat lon_step = 1.0f / lons;
    GLfloat lat_step = 1.0f / lats;
    GLuint offset = 0;
    for (int lat = 0; lat < lats; lat++) {  // 纬线u
        for (int lon = 0; lon < lons; lon++) { // 经线v
            // 一次构造4个点，两个三角形
            glm::vec3 point1 = GetPoint(lat * lat_step, lon * lon_step);
            glm::vec3 point2 = GetPoint((lat + 1) * lat_step, lon * lon_step);
            glm::vec3 point3 = GetPoint((lat + 1) * lat_step, (lon + 1) * lon_step);
            glm::vec3 point4 = GetPoint(lat * lat_step, (lon + 1) * lon_step);
            memcpy(vertices + offset, value_ptr(point1), 3 * sizeof(GLfloat));
            offset += 3;
            memcpy(vertices + offset, value_ptr(color), 4 * sizeof(GLfloat));
            offset += 4;
            memcpy(vertices + offset, value_ptr(point4), 3 * sizeof(GLfloat));
            offset += 3;
            memcpy(vertices + offset, value_ptr(color), 4 * sizeof(GLfloat));
            offset += 4;
            memcpy(vertices + offset, value_ptr(point3), 3 * sizeof(GLfloat));
            offset += 3;
            memcpy(vertices + offset, value_ptr(color), 4 * sizeof(GLfloat));
            offset += 4;

            memcpy(vertices + offset, value_ptr(point1), 3 * sizeof(GLfloat));
            offset += 3;
            memcpy(vertices + offset, value_ptr(color), 4 * sizeof(GLfloat));
            offset += 4;
            memcpy(vertices + offset, value_ptr(point3), 3 * sizeof(GLfloat));
            offset += 3;
            memcpy(vertices + offset, value_ptr(color), 4 * sizeof(GLfloat));
            offset += 4;
            memcpy(vertices + offset, value_ptr(point2), 3 * sizeof(GLfloat));
            offset += 3;
            memcpy(vertices + offset, value_ptr(color), 4 * sizeof(GLfloat));
            offset += 4;
        }
    }
    shader = new Shader("../../../shader/SMVert.vs", "../../../shader/SMFrag.frag");
    LoadSun();
}

glm::vec3 Star::GetPoint(GLfloat u, GLfloat v)
{
    constexpr GLfloat _pi = glm::pi<GLfloat>();
    GLfloat z = radius * cos(_pi * u);
    GLfloat x = radius * sin(_pi * u) * cos(2 * _pi * v);
    GLfloat y = radius * sin(_pi * u) * sin(2 * _pi * v);
    return glm::vec3(x, y, z);
}

void Star::Render(float a)
{
    glBindVertexArray(VAO);
    // 计算太阳位置并渲染
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(trackRadius * cos(a), trackRadius * sin(a), 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"), 1, GL_FALSE, value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, 6 * Star::lats * Star::lons);

    // 修改光线方向
    lightDirection.x = cos(a);
    lightDirection.y = sin(a);
    glBindVertexArray(0);
}

glm::vec3 Star::GetLightDirection()
{
    return lightDirection;
}

void Star::LoadSun()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}