#ifndef EVENT_H
#define EVENT_H

#include "Model/World.h"
#include "Model/Player.h"

class HandleList;
class Listener;
class PhysicsWorld;

// 事件基类
class Event {
public:
	virtual void addListener(Listener*) = 0;
	virtual void call() = 0;
};

// 物理模拟事件类
class PhysicsEvent : public Event {
private:
	static HandleList handleList;
	
	World* world;
	PhysicsWorld* physicsWorld;

public:
	PhysicsEvent(World*, PhysicsWorld*);
	World* getWorld();
	PhysicsWorld* getPhysicsWorld();

	void addListener(Listener*);
	void call();
};

// 键盘敲击事件类
class KeyBoardEvent : public Event{
private:
	static HandleList handleList;

	int key;
	int action;	// 1 位按下 0 为释放
	World* world;
	PhysicsWorld* physicsWorld;
	float deltaTime;

public:

	KeyBoardEvent(World*, PhysicsWorld*, int,int, float);
	World* getWorld();
	PhysicsWorld* getPhysicsWorld();
	int getKey();
	float getDeltaTime();
	int getAction() { return action; }

	void addListener(Listener*);
	void call();
};

#endif
