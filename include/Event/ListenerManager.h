#ifndef LISTENERMANAGER_H
#define LISTENERMANAGER_H

#include "Event/Listener.h"
#include <vector>

// ¼àÌıÆ÷¹ÜÀíÀà
class ListenerManager {
private:
	std::vector<Listener*> allListeners;
public:
	void registerListener(Listener*, Event*);
	~ListenerManager();
};

#endif // !LISTENERMANAGER_H