#include "Event/HandleList.h"
#include "Event/Event.h"

HandleList PhysicsEvent::handleList;

PhysicsEvent::PhysicsEvent(World* _world, PhysicsWorld* _physics) {
	world = _world;
	physicsWorld = _physics;
}

World* PhysicsEvent::getWorld() {
	return world;
}

PhysicsWorld* PhysicsEvent::getPhysicsWorld() {
	return physicsWorld;
}

/**
 * @brief 增加监听这个时间的监听器
 * @param listener 监听器
*/
void PhysicsEvent::addListener(Listener* listener) {
	handleList.addListener(listener);
}

/**
 * @brief 触发本事件
*/
void PhysicsEvent::call() {
	handleList.handl(this);
}