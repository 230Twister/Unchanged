#ifndef SUN_H
#define SUN_H

#include "Shader.h"
#include <glm/glm.hpp>

class Sun {
private:
	glm::vec3 sunLightDirection;	// 太阳光方向向量
	glm::vec4 color;
	float radius;
	float trackRadius;
public:
	static const int lats = 30; // 纬线切分
	static const int lons = 60; // 经线切分
	float vertices[6 * 7 * lats * lons];
	Sun(glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.7f, 1.0f), float radius = 10.0f, float trackr = 300.0f); // 构造函数，生成球体顶点数组
	glm::vec3 GetPoint(float u, float v); // 获取球体顶点
	void Render(Shader& sunShader, float a); // 渲染
	glm::vec3 GetLightDirection();
};

#endif // !SUN_H


