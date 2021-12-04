#include "Event/HandleList.h"
#include "Event/Event.h"

HandleList KeyBoardEvent::handleList;

KeyBoardEvent::KeyBoardEvent(World* _world, Player* _player, int _key, float _delta) {
	world = _world;
	player = _player;
	key = _key;
	deltaTime = _delta;
}

World* KeyBoardEvent::getWorld() {
	return world;
}

Player* KeyBoardEvent::getPlayer() {
	return player;
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