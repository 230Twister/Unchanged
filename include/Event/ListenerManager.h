#ifndef LISTENERMANAGER_H
#define LISTENERMANAGER_H

#include "Event/Listener.h"

// ������������
class ListenerManager {
public:
	void registerListener(Listener*, Event*);
};

#endif // !LISTENERMANAGER_H