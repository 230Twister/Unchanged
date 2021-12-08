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

	// ע�������
	listenerManager.registerListener(new KeyBoardListener(), &KeyBoardEvent(NULL, 0, 0.0f));
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
 * @brief ��Ϸ��ѭ��
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