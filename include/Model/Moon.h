#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

class Moon {
private:
	glm::vec3 moonLightDirection;	// ̫���ⷽ������
	glm::vec4 color;
	float radius;
	float trackRadius;
public:
	static const int lats = 30; // γ���з�
	static const int lons = 60; // �����з�
	GLfloat vertices[6 * 7 * lats * lons];
	Moon(glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), GLfloat radius = 20.0f, GLfloat trackr = 400.0f);
	glm::vec3 GetPoint(GLfloat u, GLfloat v);
	void Render(Shader& moonShader, float a);
	glm::vec3 GetLightDirection();
};