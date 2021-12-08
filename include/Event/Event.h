#ifndef EVENT_H
#define EVENT_H

#include "Model/World.h"
#include "Model/Player.h"

class HandleList;
class Listener;
class PhysicsWorld;

// �¼�����
class Event {
public:
	virtual void addListener(Listener*) = 0;
	virtual void call() = 0;
};

// ����ģ���¼���
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

// �����û��¼���
class KeyBoardEvent : public Event{
private:
	static HandleList handleList;

	int key;
	World* world;
	float deltaTime;

public:

	KeyBoardEvent(World*, int, float);
	World* getWorld();
	int getKey();
	float getDeltaTime();

	void addListener(Listener*);
	void call();
};

#endif
