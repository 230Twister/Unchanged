#ifndef SUN_H
#define SUN_H

#include "Shader.h"
#include <glm/glm.hpp>

class Sun {
private:
	glm::vec3 sunLightDirection;	// ̫���ⷽ������
	glm::vec4 color;
	float radius;
	float trackRadius;
public:
	static const int lats = 30; // γ���з�
	static const int lons = 60; // �����з�
	float vertices[6 * 7 * lats * lons];
	Sun(glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.7f, 1.0f), float radius = 10.0f, float trackr = 300.0f); // ���캯�����������嶥������
	glm::vec3 GetPoint(float u, float v); // ��ȡ���嶥��
	void Render(Shader& sunShader, float a); // ��Ⱦ
	glm::vec3 GetLightDirection();
};

#endif // !SUN_H


