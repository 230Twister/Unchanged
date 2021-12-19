#pragma once

#include "Model.h"
#include "glm/glm.hpp"

class Weapon {
private:
	Model* model;
	float yaw;
	float angle;
	bool direction;
	float length;
	float speed;
	float range;
	
	glm::vec3 position;
public:
	Weapon();

	void Render(Shader*);
	void SetPosition(glm::vec3);
	void SetYaw(float);
};