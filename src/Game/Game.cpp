#include "Game/Game.h"
#include "Event/Event.h"
#include "Event/Listener.h"
#include "Event/HandleList.h"
#include "Model/Zombie.h"

/**
 * @brief 游戏初始化
*/
void Game::init() {
	world = new World("../../../world_model/models.obj");
	player = new Player();
	physics = new PhysicsWorld();
	
	physics->addRigidBody(world->getBaseModel());
	physics->addCharator(btVector3(0, 50, 0), 0);
	
	world->addPlayer(player);
	world->setCamera(player->getCamera());

	// 添加一个僵尸
	Zombie* zombie = new Zombie(glm::vec3(10, 50, 0), 0.0);
	zombie->init();
	world->addZombie(zombie);
	physics->addCharator(btVector3(10, 50, 0), 1);

	float zombieYaws[10]{270, 90, -10, 20, 180, -90, 40, 66 , 60, 70};

	glm::vec3 zombiePosition[10]{
		{15, 40, 30}, {10, 10, 10},
		{22, 100, 20}, {10, 55, 15},
		{10, 50, 25}, {19, 45, 35},
		{10, 30, -10}, {30, 70, -12},
		{10, 35, -20}, {20, 60, 0}
	};

	// 添加一堆僵尸
	for (int i = 0; i < 10; i++) {
		Zombie* zombie = new Zombie(zombiePosition[i], zombieYaws[i]);
		world->addZombie(zombie);
		physics->addCharator(btVector3(zombiePosition[i].x, zombiePosition[i].y, zombiePosition[i].z), 2 + i);
	}

	// 注册监听器
	listenerManager.registerListener(new KeyBoardListener(), &KeyBoardEvent(NULL, NULL, 0, 0, 0.0f));
	listenerManager.registerListener(new PhysicsListener(), &PhysicsEvent(NULL, NULL));
	listenerManager.registerListener(new AttackListener(), &AttackEvent(NULL, NULL, 0));
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
		KeyBoardEvent(world, physics, GLFW_KEY_G, 1, deltaTime).call();
	}
	else if(glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
		KeyBoardEvent(world, physics, GLFW_KEY_G, 0, deltaTime).call();
	}
}

/**
 * @brief 游戏主循环
*/
void Game::loop() {
	GLfloat currentFrame = glfwGetTime();

	// 物理世界模拟
	physics->stepSimulation();
	PhysicsEvent(world, physics).call();

	// 玩家攻击与被攻击检测
	int attackZombie = physics->attackTest(player);
	if (attackZombie && player->canAttack(currentFrame)) {
		AttackEvent(world, physics, attackZombie).call();
		player->disableAttack();
	}
	int beAttacked = physics->attackedTest();
	if (beAttacked) {
		AttackEvent(world, physics, -beAttacked).call();
	}

	// 模型世界渲染
	world->renderDepthMap();
	world->render();

	// 时间更新
	if (currentFrame - frame >= 0.1f) {
		frame = currentFrame;
		world->setTime(world->getTime() + 1);
	}
}

/**
 * @brief 让玩家发起一次攻击
*/
void Game::playerAttack() {
	player->attack(glfwGetTime());
}

Game::~Game() {
	delete world;
	delete physics;
	delete player;
}