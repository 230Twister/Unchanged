#ifndef GAME_H
#define GAME_H

#include "Model/World.h"
#include "PhysicsWorld.h"
#include "Model/Player.h"
#include "Event/ListenerManager.h"
#include <GLFW/glfw3.h>

class Zombie;

class Game {
private:
	World* world;
	PhysicsWorld* physics;
	Player* player;

	ListenerManager listenerManager;

	float frame = 0.0f;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
public:
	void init();

	void processInput(GLFWwindow*);
	void loop();
	Camera* getCamera() {
		return player->getCamera();
	}
	~Game();
};

#endif