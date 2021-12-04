#include "Model/Player.h"

Player::Player() {
	camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));
	model = NULL;
}

Camera* Player::getCamera() {
	return camera;
}

Player::~Player() {
	delete camera;
}