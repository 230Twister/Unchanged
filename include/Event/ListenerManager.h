#ifndef LISTENERMANAGER_H
#define LISTENERMANAGER_H

#include "Event/Listener.h"

// ¼àÌıÆ÷¹ÜÀíÀà
class ListenerManager {
private:
	std::vector<Listener*> allListeners;
public:
	void registerListener(Listener*, Event*);
	~ListenerManager();
};

#endif // !LISTENERMANAGER_H