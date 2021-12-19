#include "Model/Zombie.h"
#include "Model/Model.h"
#include "Shader.h"

Model* Zombie::model = NULL;

Zombie::Zombie(glm::vec3 pos) {
	position = pos;
	health = 50;
	dead = false;
}

void Zombie::init() {
	model = new Model("../../../world_model/player/zb.obj");
}

/**
 * @brief ��Ⱦ��ʬģ��
 * @param shader 
*/
void Zombie::render(Shader* shader) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::translate(mat, position);

	shader->setMat4("model", mat);

	model->Draw(*shader);
}

/**
 * @brief ���ý�ʬѪ��
 * @param health Ѫ��
*/
void Zombie::setHealth(int health) {
	this->health = health;

	if (health <= 0) {
		dead = true;
	}
}

/**
 * @brief ��ȡ��ʬѪ��
 * @return Ѫ��
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
