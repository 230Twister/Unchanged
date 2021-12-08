#ifndef MOON_H
#define MOON_H

#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Moon {
private:
	glm::vec3 moonLightDirection;	// 太阳光方向向量
	glm::vec4 color;
	float radius;
	float trackRadius;

	unsigned int moonVAO;
	unsigned int moonVBO;

	void LoadMoon();
public:
	Shader* moonShader;			// 月亮着色器
	static const int lats = 30; // 纬线切分
	static const int lons = 60; // 经线切分
	GLfloat vertices[6 * 7 * lats * lons];
	Moon(glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), GLfloat radius = 20.0f, GLfloat trackr = 400.0f);
	glm::vec3 GetPoint(GLfloat u, GLfloat v);
	void Render(float a);
	glm::vec3 GetLightDirection();
};

#endif // !MOON_H