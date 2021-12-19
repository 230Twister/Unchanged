#include "Model/Zombie.h"
#include "Model/Model.h"
#include "Shader.h"

Model* Zombie::model = NULL;

Zombie::Zombie(glm::vec3 pos) {
	position = pos;
	health = 20;
}

void Zombie::init() {
	model = new Model("../../../world_model/player/zb.obj");
}

/**
 * @brief 渲染僵尸模型
 * @param shader 
*/
void Zombie::render(Shader* shader) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::translate(mat, position);

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

	}
}

/**
 * @brief 获取僵尸血量
 * @return 血量
*/
int Zombie::getHealth() {
	return health;
}

void Zombie::setPosition(glm::vec3 pos){
	position = pos;
}
