#ifndef MOON_H
#define MOON_H

#include <glm/glm.hpp>
#include "Shader.h"

class Moon {
private:
	glm::vec3 moonLightDirection;	// 太阳光方向向量
	glm::vec4 color;
	float radius;
	float trackRadius;
public:
	static const int lats = 30; // 纬线切分
	static const int lons = 60; // 经线切分
	float vertices[6 * 7 * lats * lons];
	Moon(glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), float radius = 10.0f, float trackr = 300.0f);
	glm::vec3 GetPoint(float u, float v);
	void Render(Shader& moonShader, float a);
	glm::vec3 GetLightDirection();
};

#endif // !MOON_H

