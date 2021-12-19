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
 * @brief ���Ӽ������ʱ��ļ�����
 * @param listener ������
*/
void AttackEvent::addListener(Listener* listener) {
	handleList.addListener(listener);
}

/**
 * @brief �������¼�
*/
void AttackEvent::call() {
	handleList.handl(this);
}