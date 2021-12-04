#include "Game/Game.h"
#include "Event/Event.h"

void Game::init() {
	world = new World("../../../world_model/models.obj");
	physics = new PhysicsWorld();
	physics->addRigidBody(world->getBaseModel());
	player = new Player();

	world->setCamera(player->getCamera());

	// 注册监听器
	listenerManager.registerListener(new KeyBoardListener(), &KeyBoardEvent(NULL, NULL, 0, 0.0f));
}

void Game::processInput(GLFWwindow* window) {

	// 更新时间差
	GLfloat currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		KeyBoardEvent event(world, player, GLFW_KEY_W, deltaTime);
		event.call();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		KeyBoardEvent event(world, player, GLFW_KEY_S, deltaTime);
		event.call();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		KeyBoardEvent event(world, player, GLFW_KEY_A, deltaTime);
		event.call();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		KeyBoardEvent event(world, player, GLFW_KEY_D, deltaTime);
		event.call();
	}
}

void Game::loop() {
	world->renderDepthMap();
	world->render();
}

Game::~Game() {
	delete world;
	delete physics;
	delete player;
}