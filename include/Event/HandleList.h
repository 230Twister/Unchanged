#ifndef HANDLELIST_H
#define HANDLELIST_H

#include <vector>
#include "Event/Listener.h"

// �¼�������
class HandleList {
private:
	std::vector<Listener*> listeners;
public:
	void addListener(Listener*);
	void handl(Event*);
};

#endif // !HANDLELIST_H
