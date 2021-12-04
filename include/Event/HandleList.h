#ifndef HANDLELIST_H
#define HANDLELIST_H

#include <vector>
#include "Event/Listener.h"

// 事件处理类
class HandleList {
private:
	std::vector<Listener*> listeners;
public:
	void addListener(Listener*);
	void handl(Event*);
};

#endif // !HANDLELIST_H
