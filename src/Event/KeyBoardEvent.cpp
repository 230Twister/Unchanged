#include "Event/HandleList.h"
#include "Event/Event.h"
#include "Game/PhysicsWorld.h"

HandleList KeyBoardEvent::handleList;

KeyBoardEvent::KeyBoardEvent(World* _world, PhysicsWorld* _physics, int _key, float _delta) {
	world = _world;
	physicsWorld = _physics;
	key = _key;
	deltaTime = _delta;
}

World* KeyBoardEvent::getWorld() {
	return world;
}

PhysicsWorld* KeyBoardEvent::getPhysicsWorld() {
	return physicsWorld;
}

int KeyBoardEvent::getKey() {
	return key;
}

float KeyBoardEvent::getDeltaTime() {
	return deltaTime;
}

/**
 * @brief ���Ӽ������ʱ��ļ�����
 * @param listener ������
*/
void KeyBoardEvent::addListener(Listener* listener) {
	handleList.addListener(listener);
}

/**
 * @brief �������¼�
*/
void KeyBoardEvent::call() {
	handleList.handl(this);
}