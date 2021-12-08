#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

class Sun {
private:
	glm::vec3 sunLightDirection;	// 太阳光方向向量
	glm::vec4 color;
	float radius;
	float trackRadius;
public:
	static const int lats = 30; // 纬线切分
	static const int lons = 60; // 经线切分
	GLfloat vertices[6 * 7 * lats * lons];
	Sun(glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.7f, 1.0f), GLfloat radius = 20.0f, GLfloat trackr = 400.0f); // 构造函数，生成球体顶点数组
	glm::vec3 GetPoint(GLfloat u, GLfloat v); // 获取球体顶点
	void Render(Shader& sunShader, float a); // 渲染
	glm::vec3 GetLightDirection();
};