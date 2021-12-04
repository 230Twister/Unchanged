#ifndef LISTENERMANAGER_H
#define LISTENERMANAGER_H

#include "Event/Listener.h"

// ¼àÌıÆ÷¹ÜÀíÀà
class ListenerManager {
public:
	void registerListener(Listener*, Event*);
};

#endif // !LISTENERMANAGER_H