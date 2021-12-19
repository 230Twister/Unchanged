#include "Event/Event.h"
#include "Event/HandleList.h"
#include "Game/PhysicsWorld.h"

HandleList AttackEvent::handleList;

AttackEvent::AttackEvent(World* _world, PhysicsWorld* _physics, int type) {
	world = _world;
	physicsWorld = _physics;
	attackType = type;
}

World* AttackEvent::getWorld() {
	return world;
}

PhysicsWorld* AttackEvent::getPhysicsWorld() {
	return physicsWorld;
}

int AttackEvent::getAttackType() {
	return attackType;
}

/**
 * @brief 增加监听这个时间的监听器
 * @param listener 监听器
*/
void AttackEvent::addListener(Listener* listener) {
	handleList.addListener(listener);
}

/**
 * @brief 触发本事件
*/
void AttackEvent::call() {
	handleList.handl(this);
}