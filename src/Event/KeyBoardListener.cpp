#include "Event/Event.h"
#include "Event/HandleList.h"
#include "Game/PhysicsWorld.h"
#include <GLFW/glfw3.h>

void KeyBoardListener::setEvent(Event* event) {
	keyBoardEvent = static_cast<KeyBoardEvent*>(event);
}

void KeyBoardListener::handle() {
	Camera* camera = keyBoardEvent->getWorld()->getCamera();
	PhysicsWorld* physics = keyBoardEvent->getPhysicsWorld();
	int key = keyBoardEvent->getKey();
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
	else {
		physics->characterStop();
	}
}