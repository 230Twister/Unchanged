#include <Event/HandleList.h>

void HandleList::addListener(Listener* listener) {
	listeners.push_back(listener);
}

/**
 * @brief 通知各个监听器，开始处理事件
 * @param event 待处理的事件
*/
void HandleList::handl(Event* event) {
	for (Listener* listener : listeners) {
		listener->setEvent(event);
		listener->handle();
	}
}