#include "Model/Player.h"
#include "Game/PhysicsWorld.h"

Player::Player() {
	camera = new Camera(glm::vec3(0.0f, 50.0f, 0.0f));
	model = new Model("../../../world_model/player/models.obj");
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	health = 20;
}

/**
 * @brief ��Ⱦ���ģ��
 * @param shader ʹ�õ���ɫ��
*/
void Player::render(Shader* shader) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::translate(mat, position);

	shader->setMat4("model", mat);

	model->Draw(*shader);
}

void Player::setPosition(glm::vec3 pos) {
	position = pos;
}

Camera* Player::getCamera() {
	return camera;
}

Player::~Player() {
	delete camera;
	delete model;
}