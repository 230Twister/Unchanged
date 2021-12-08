#include "Event/ListenerManager.h"
#include "Event/Event.h"

void ListenerManager::registerListener(Listener* listener, Event* event) {
	event->addListener(listener);
	allListeners.push_back(listener);
}

ListenerManager::~ListenerManager() {
	for (Listener* listener : allListeners) {
		delete listener;
	}
}