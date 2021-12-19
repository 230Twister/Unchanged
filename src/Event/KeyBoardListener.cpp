#include "Event/Event.h"
#include "Event/HandleList.h"
#include "Game/PhysicsWorld.h"
#include "Model/World.h"
#include <GLFW/glfw3.h>

void KeyBoardListener::setEvent(Event* event) {
	keyBoardEvent = static_cast<KeyBoardEvent*>(event);
}

void KeyBoardListener::handle() {
	Camera* camera = keyBoardEvent->getWorld()->getCamera();
	PhysicsWorld* physics = keyBoardEvent->getPhysicsWorld();
	int key = keyBoardEvent->getKey();
	int action = keyBoardEvent->getAction();
	float delta_time = keyBoardEvent->getDeltaTime();

	if (key == GLFW_KEY_W) {
		physics->updateCharacterFront(camera->Yaw);
		physics->characterWalk(WalkDirection::UP, delta_time);
	}
	else if (key == GLFW_KEY_S) {
		physics->updateCharacterFront(camera->Yaw);
		physics->characterWalk(WalkDirection::DOWN, delta_time);
	}
	else if (key == GLFW_KEY_A) {
		physics->updateCharacterFront(camera->Yaw);
		physics->characterWalk(WalkDirection::LEFT, delta_time);
	}
	else if (key == GLFW_KEY_D) {
		physics->updateCharacterFront(camera->Yaw);
		physics->characterWalk(WalkDirection::RIGHT, delta_time);
	}
	else if (key == GLFW_KEY_SPACE) {
		physics->characterJump();
	}
	else if (key == GLFW_KEY_G) {
		if (action == 1) {
			keyBoardEvent->getWorld()->getPlayer()->transViewMode();
		}else {
			keyBoardEvent->getWorld()->getPlayer()->transEna();
		}
	}
	else if (key == GLFW_KEY_H) {
		if (action == 1) {
			keyBoardEvent->getWorld()->getPlayer()->transFlashMode();
		}
		else {
			keyBoardEvent->getWorld()->getPlayer()->transFlashEna();
		}
	}
	else {
		physics->characterStop();
	}
}