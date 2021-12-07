#include "Event/Event.h"
#include "Event/HandleList.h"
#include <GLFW/glfw3.h>

void KeyBoardListener::setEvent(Event* event) {
	keyBoardEvent = static_cast<KeyBoardEvent*>(event);
}

void KeyBoardListener::handle() {
	Camera* camera = keyBoardEvent->getWorld()->getCamera();
	int key = keyBoardEvent->getKey();
	float delta_time = keyBoardEvent->getDeltaTime();

	if (key == GLFW_KEY_W) {
		camera->ProcessKeyboard(FORWARD, delta_time);
	}
	else if (key == GLFW_KEY_S) {
		camera->ProcessKeyboard(BACKWARD, delta_time);
	}
	else if (key == GLFW_KEY_A) {
		camera->ProcessKeyboard(LEFT, delta_time);
	}
	else if (key == GLFW_KEY_D) {
		camera->ProcessKeyboard(RIGHT, delta_time);
	}
		
}