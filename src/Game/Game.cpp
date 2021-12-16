#include "Game/Game.h"
#include "Event/Event.h"
#include "Event/Listener.h"
#include "Event/HandleList.h"

/**
 * @brief 游戏初始化
*/
void Game::init() {
	world = new World("../../../world_model/models.obj");
	player = new Player();
	physics = new PhysicsWorld();
	
	physics->addRigidBody(world->getBaseModel());
	physics->addCharator(player->getBaseModel(), btVector3(0, 50, 0));
	
	world->addEntity(player);
	world->setCamera(player->getCamera());

	// 注册监听器
	listenerManager.registerListener(new KeyBoardListener(), &KeyBoardEvent(NULL, NULL, 0, 0, 0.0f));
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

	physics->characterStop();
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		KeyBoardEvent(world, physics, GLFW_KEY_W, 1, deltaTime).call();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		KeyBoardEvent(world, physics, GLFW_KEY_S, 1,deltaTime).call();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		KeyBoardEvent(world, physics, GLFW_KEY_A,1, deltaTime).call();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		KeyBoardEvent(world, physics, GLFW_KEY_D, 1,deltaTime).call();
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		KeyBoardEvent(world, physics, GLFW_KEY_SPACE,1, deltaTime).call();
	}

	if (glfwGetKey(window, GLFW_KEY_G) ==  GLFW_PRESS) {
		KeyBoardEvent(world, physics, GLFW_KEY_G,1, deltaTime).call();
	}else if(glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
		KeyBoardEvent(world, physics, GLFW_KEY_G, 0, deltaTime).call();
	}
}

/**
 * @brief 游戏主循环
*/
void Game::loop() {
	// 物理世界模拟
	physics->stepSimulation();
	PhysicsEvent(world, physics).call();

	// 模型世界渲染
	world->renderDepthMap();
	world->render();

	// 时间更新
	GLfloat currentFrame = glfwGetTime();
	if (currentFrame - frame >= 0.1f) {
		frame = currentFrame;
		world->setTime(world->getTime() + 1);
	}
}

Game::~Game() {
	delete world;
	delete physics;
	delete player;
}