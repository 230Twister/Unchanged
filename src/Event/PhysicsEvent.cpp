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
 * @brief ���Ӽ������ʱ��ļ�����
 * @param listener ������
*/
void PhysicsEvent::addListener(Listener* listener) {
	handleList.addListener(listener);
}

/**
 * @brief �������¼�
*/
void PhysicsEvent::call() {
	handleList.handl(this);
}