#ifndef PLAYER_H
#define PLAYER_H

#include "Model.h"
#include "Camera.h"
#include "glm/glm.hpp"

class PhysicsWorld;

class Player {
private:
	Model* model;
	Camera* camera;

	glm::vec3 position;	// �����������
	int health;			// Ѫ��

public:
	Player();

	void render(Shader*);

	void setPosition(glm::vec3);

	Camera* getCamera();
	Model* getBaseModel() { return model; }
	~Player();
};

#endif