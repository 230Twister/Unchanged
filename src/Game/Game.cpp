#include "Game/Game.h"
#include "Event/Event.h"
#include "Event/Listener.h"
#include "Event/HandleList.h"

/**
 * @brief ��Ϸ��ʼ��
*/
void Game::init() {
	world = new World("../../../world_model/models.obj");
	player = new Player();
	physics = new PhysicsWorld();
	
	physics->addRigidBody(world->getBaseModel());
	physics->addCharator(player->getBaseModel(), btVector3(0, 50, 0));
	
	world->addEntity(player);
	world->setCamera(player->getCamera());

	// ע�������
	listenerManager.registerListener(new KeyBoardListener(), &KeyBoardEvent(NULL, NULL, 0, 0.0f));
	listenerManager.registerListener(new PhysicsListener(), &PhysicsEvent(NULL, NULL));
}

/**
 * @brief ���������������
 * @param window ����
*/
void Game::processInput(GLFWwindow* window) {

	// ����ʱ���
	GLfloat currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	physics->characterStop();
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		KeyBoardEvent(world, physics, GLFW_KEY_W, deltaTime).call();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		KeyBoardEvent(world, physics, GLFW_KEY_S, deltaTime).call();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		KeyBoardEvent(world, physics, GLFW_KEY_A, deltaTime).call();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		KeyBoardEvent(world, physics, GLFW_KEY_D, deltaTime).call();
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		KeyBoardEvent(world, physics, GLFW_KEY_SPACE, deltaTime).call();
	}
}

/**
 * @brief ��Ϸ��ѭ��
*/
void Game::loop() {
	// ��������ģ��
	physics->stepSimulation();
	PhysicsEvent(world, physics).call();

	// ģ��������Ⱦ
	world->renderDepthMap();
	world->render();

	// ʱ�����
	GLfloat currentFrame = glfwGetTime();
	if (currentFrame - frame >= 0.2f) {
		frame = currentFrame;
		world->setTime(world->getTime() + 1);
	}
}

Game::~Game() {
	delete world;
	delete physics;
	delete player;
}