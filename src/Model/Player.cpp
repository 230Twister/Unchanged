#include "Model/Player.h"
#include "Game/PhysicsWorld.h"

Player::Player() {
	camera = new Camera(glm::vec3(0.0f, 50.0f, 0.0f));
	model = new Model("../../../world_model/player/player.obj");
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	health = 100;
	view_mode = 1;
	view_mode_trans_ena = 1;
	yaw = 0.0f;
}

/**
 * @brief 渲染玩家模型
 * @param shader 使用的着色器
*/
void Player::render(Shader* shader) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::translate(mat, position);
	mat = glm::rotate(mat, glm::radians(yaw), glm::vec3(0, 1, 0));

	shader->setMat4("model", mat);

	model->Draw(*shader);
}

/**
 * @brief 设置玩家血量
 * @param health 血量
*/
void Player::setHealth(int health) {
	this->health = health;

	if (health <= 0) {

	}
}

/**
 * @brief 获取玩家血量
 * @return 血量
*/
int Player::getHealth() {
	return health;
}

glm::vec3 Player::getPosition() {
	return this->position;
}

void Player::transViewMode() {
	if(view_mode_trans_ena)	view_mode = 1 - view_mode;
	view_mode_trans_ena = 0;
}

void Player::transEna() {
	view_mode_trans_ena = 1;
}

void Player::setPosition(glm::vec3 pos) {
	position = pos;
}

void Player::setYaw(float yaw) {
	this->yaw = yaw;
}

Camera* Player::getCamera() {
	return camera;
}

Player::~Player() {
	delete camera;
	delete model;
}