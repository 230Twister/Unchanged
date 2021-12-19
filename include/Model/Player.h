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
	float yaw;
	int health;			// Ѫ��
	
	int view_mode;
	int view_mode_trans_ena;

	float attackTime;
	bool attackState;

public:
	Player();

	void render(Shader*);

	void setPosition(glm::vec3);
	void setYaw(float);
	void transViewMode();
	void transEna();
	void setHealth(int);
	int getHealth();
	
	void attack(float);
	bool canAttack(float);
	void disableAttack();

	glm::vec3 getPosition();
	Camera* getCamera();
	Model* getBaseModel() { return model; }
	int getViewMode() { return view_mode; }
	~Player();
};

#endif