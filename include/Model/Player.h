#ifndef PLAYER_H
#define PLAYER_H

#include "Model.h"
#include "Camera.h"

class Player {
private:
	Model* model;
	Camera* camera;

	int health;			// Ѫ��

public:
	Player();

	Camera* getCamera();
	~Player();
};

#endif