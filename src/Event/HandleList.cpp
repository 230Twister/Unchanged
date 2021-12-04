#include <Event/HandleList.h>

void HandleList::addListener(Listener* listener) {
	listeners.push_back(listener);
}

/**
 * @brief ֪ͨ��������������ʼ�����¼�
 * @param event ��������¼�
*/
void HandleList::handl(Event* event) {
	for (Listener* listener : listeners) {
		listener->setEvent(event);
		listener->handle();
	}
}