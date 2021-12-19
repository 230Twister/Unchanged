#include "Model/Player.h"
#include "Game/PhysicsWorld.h"
#include "GLFW/glfw3.h"

Player::Player() {
	camera = new Camera(glm::vec3(0.0f, 50.0f, 0.0f));
	model = new Model("../../../world_model/player/player.obj");
	knife = new Weapon;
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	health = 100;
	attackState = true;
	view_mode = 1;
	view_mode_trans_ena = 1;
	flash_mode = 0;
	flash_mode_trans_ena = 1;
	yaw = 0.0f;
}

/**
 * @brief ��Ⱦ���ģ��
 * @param shader ʹ�õ���ɫ��
*/
void Player::render(Shader* shader) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::translate(mat, position);
	mat = glm::rotate(mat, glm::radians(yaw), glm::vec3(0, 1, 0));

	shader->setMat4("model", mat);

	model->Draw(*shader);

	// ��Ⱦ����
	knife->SetPosition(position + glm::vec3(0.35f * glm::cos(glm::radians(25-yaw)), 0.35f, 0.35f * glm::sin(glm::radians(25-yaw))));
	knife->SetYaw(yaw);
	float now_time = glfwGetTime();
	knife->Render(shader, attackTime - now_time < 0 ? 0 : attackTime - now_time);
}

/**
 * @brief �������Ѫ��
 * @param health Ѫ��
 * @return �Ƿ�����
*/
bool Player::setHealth(int health) {
	this->health = health;

	if (health <= 0) {
		this->health = 100;
		return true;
	}
	return false;
}

/**
 * @brief ��ȡ���Ѫ��
 * @return Ѫ��
*/
int Player::getHealth() {
	return health;
}

/**
 * @brief ��ҿ�ʼ����
 * @param time ��ǰglfwʱ��
*/
void Player::attack(float time) {
	if (attackTime <= time) {
		// ����Ӳֱ���
		attackTime = time + 2.0f;
		attackState = true;
	}
}

/**
 * @brief �ڹ���ʱ���ڹ���һ�κ�����κ�������
*/
void Player::disableAttack() {
	attackState = false;
}

/**
 * @brief �жϵ�ǰ����ܷ���ɹ���
 * @return �ܷ���ɹ���
*/
bool Player::canAttack(float time) {
	if (!attackState)
		return false;
	if (attackTime < time)
		return false;
	return true;
}

glm::vec3 Player::getPosition() {
	return this->position;
}

void Player::transViewMode() {
	if(view_mode_trans_ena)	view_mode = 1 - view_mode;
	view_mode_trans_ena = 0;
}

void Player::transFlashMode() {
	if (flash_mode_trans_ena) flash_mode = 1 - flash_mode;
	flash_mode_trans_ena = 0;
}

void Player::transEna() {
	view_mode_trans_ena = 1;
}

void Player::transFlashEna() {
	flash_mode_trans_ena = 1;
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
	delete knife;
}