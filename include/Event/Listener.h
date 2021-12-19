#ifndef LISTENER_H
#define LISTENER_H

class Event;
class KeyBoardEvent;
class PhysicsEvent;
class AttackEvent;

class Listener {
public:
	virtual void setEvent(Event*) = 0;
	virtual void handle() = 0;
};

// 攻击监听类
class AttackListener : public Listener {
private:
	AttackEvent* attackEvent;
public:
	void setEvent(Event*);
	void handle();
};

// 物理模拟监听类
class PhysicsListener : public Listener {
private:
	PhysicsEvent* physicsEvent;
public:
	void setEvent(Event*);
	void handle();
};

// 键盘监听类
class KeyBoardListener : public Listener {
private:
	KeyBoardEvent* keyBoardEvent;
public:
	void setEvent(Event*);
	void handle();
};

#endif
