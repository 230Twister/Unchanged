#include "Event/ListenerManager.h"
#include "Event/Event.h"

void ListenerManager::registerListener(Listener* listener, Event* event) {
	event->addListener(listener);
}