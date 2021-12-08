#include "Model/Player.h"

Player::Player() {
	camera = new Camera(glm::vec3(103.0f, 8.0f, 30.0f));
	// camera = new Camera(glm::vec3(0.0f, 10.0f, 20.0f));
	model = NULL;
}

Camera* Player::getCamera() {
	return camera;
}

Player::~Player() {
	delete camera;
}