#ifndef SUN_H
#define SUN_H

#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Sun {
private:
	glm::vec3 sunLightDirection;	// ̫���ⷽ������
	glm::vec4 color;
	float radius;
	float trackRadius;
	unsigned int sunVAO;
	unsigned int sunVBO;

public:
	Shader* sunShader;				// ̫����ɫ��
	static const int lats = 30; // γ���з�
	static const int lons = 60; // �����з�
	GLfloat vertices[6 * 7 * lats * lons];
	Sun(glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.7f, 1.0f), GLfloat radius = 20.0f, GLfloat trackr = 400.0f); // ���캯�����������嶥������
	void LoadSun();
	glm::vec3 GetPoint(GLfloat u, GLfloat v); // ��ȡ���嶥��
	void Render(float a); // ��Ⱦ
	glm::vec3 GetLightDirection();
};

#endif // !SUN_H