#include "Model/Zombie.h"
#include "Model/Model.h"
#include "Shader.h"

Model* Zombie::model = NULL;

Zombie::Zombie(glm::vec3 pos, float y) {
	position = pos;
	health = 50;
	dead = false;
	yaw = y;
}

void Zombie::init() {
	model = new Model("world_model/player/zb.obj");
}

/**
 * @brief 渲染僵尸模型
 * @param shader 
*/
void Zombie::render(Shader* shader) {
	glm::mat4 mat = glm::mat4(1.0f);
	if (dead) {
		mat = glm::translate(mat, glm::vec3(position.x, position.y - 0.5f, position.z));
	}
	else {
		mat = glm::translate(mat, position);
	}
	mat = glm::rotate(mat, glm::radians(yaw), glm::vec3(0, 1, 0));
	if (dead) {
		mat = glm::rotate(mat, glm::radians((float)90.0), glm::vec3(0, 0, 1));
	}
	shader->setMat4("model", mat);

	model->Draw(*shader);
}

/**
 * @brief 设置僵尸血量
 * @param health 血量
*/
void Zombie::setHealth(int health) {
	this->health = health;

	if (health <= 0) {
		dead = true;
	}
}

/**
 * @brief 获取僵尸血量
 * @return 血量
*/
int Zombie::getHealth() {
	return health;
}

bool Zombie::isDead() {
	return dead;
}

void Zombie::setPosition(glm::vec3 pos){
	position = pos;
}
