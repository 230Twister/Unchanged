#ifndef LISTENER_H
#define LISTENER_H

class Event;
class KeyBoardEvent;

class Listener {
public:
	virtual void setEvent(Event*) = 0;
	virtual void handle() = 0;
};

class KeyBoardListener : public Listener {
private:
	KeyBoardEvent* keyBoardEvent;
public:
	void setEvent(Event*);
	void handle();
};

#endif
