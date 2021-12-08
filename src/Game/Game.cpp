#include "Game/Game.h"
#include "Event/Event.h"
#include "Event/Listener.h"
#include "Event/HandleList.h"

void Game::init() {
	world = new World("../../../world_model/models.obj");
	player = new Player();
	physics = new PhysicsWorld();
	
	physics->addRigidBody(world->getBaseModel());
	physics->addMoveingRigidBody(player->getBaseModel(), 1.0f, btVector3(-15, 50, 0));
	
	world->addEntity(player);
	world->setCamera(player->getCamera());

	// 注册监听器
	listenerManager.registerListener(new KeyBoardListener(), &KeyBoardEvent(NULL, 0, 0.0f));
	listenerManager.registerListener(new PhysicsListener(), &PhysicsEvent(NULL, NULL));
}

/**
 * @brief 处理键盘输入问题
 * @param window 窗口
*/
void Game::processInput(GLFWwindow* window) {

	// 更新时间差
	GLfloat currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		KeyBoardEvent event(world, GLFW_KEY_W, deltaTime);
		event.call();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		KeyBoardEvent event(world, GLFW_KEY_S, deltaTime);
		event.call();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		KeyBoardEvent event(world, GLFW_KEY_A, deltaTime);
		event.call();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		KeyBoardEvent event(world, GLFW_KEY_D, deltaTime);
		event.call();
	}
}

/**
 * @brief 游戏主循环
*/
void Game::loop() {
	physics->stepSimulation();
	PhysicsEvent(world, physics).call();

	world->renderDepthMap();
	world->render();

}

Game::~Game() {
	delete world;
	delete physics;
	delete player;
}