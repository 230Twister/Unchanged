#ifndef PLAYER_H
#define PLAYER_H

#include "Model.h"
#include "Camera.h"
#include "Weapon.h"
#include "glm/glm.hpp"

class PhysicsWorld;

class Player {
private:
	Model* model;
	Camera* camera;
	Weapon* knife;

	glm::vec3 position;	// �����������
	float yaw;
	int health;			// Ѫ��
	
	// �˳�ת��
	int view_mode;
	int view_mode_trans_ena;

	// �ֵ�Ͳ����
	int flash_mode;
	int flash_mode_trans_ena;

	float attackTime;
	bool attackState;

public:
	Player();

	void render(Shader*);

	void setPosition(glm::vec3);
	void setYaw(float);
	void transViewMode();
	void transEna();
	void transFlashMode();
	void transFlashEna();
	bool setHealth(int);
	int getHealth();
	bool isDying() { return health <= 10; }
	
	void attack(float);
	bool canAttack(float);
	void disableAttack();

	glm::vec3 getPosition();
	Camera* getCamera();
	Model* getBaseModel() { return model; }
	int getViewMode() { return view_mode; }
	int getFlashMode() { return flash_mode; }
	~Player();
};

#endif