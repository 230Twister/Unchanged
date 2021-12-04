#ifndef EVENT_H
#define EVENT_H

#include "Model/World.h"
#include "Model/Player.h"

class HandleList;
class Listener;

// 事件基类
class Event {
public:
	virtual void addListener(Listener*) = 0;
	virtual void call() = 0;
};

// 键盘监听类
class KeyBoardEvent : public Event{
private:
	static HandleList handleList;

	int key;
	World* world;
	Player* player;
	float deltaTime;

public:

	KeyBoardEvent(World*, Player*, int, float);
	World* getWorld();
	Player* getPlayer();
	int getKey();
	float getDeltaTime();

	void addListener(Listener*);
	void call();
};

#endif
