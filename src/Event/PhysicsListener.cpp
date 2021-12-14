#include "Event/Event.h"
#include "Event/HandleList.h"
#include "Game/PhysicsWorld.h"

void PhysicsListener::setEvent(Event* event) {
	physicsEvent = static_cast<PhysicsEvent*>(event);
}

/**
 * @brief ͬ��������������Ⱦ����
*/
void PhysicsListener::handle() {
	PhysicsWorld* physics = physicsEvent->getPhysicsWorld();
	World* world = physicsEvent->getWorld();

	btTransform& transform = physics->getTransform(1);
	glm::vec3 position = glm::vec3(transform.getOrigin().getX(), transform.getOrigin().getY(), transform.getOrigin().getZ());
	
	Player* player = world->getPlayer();
	player->setPosition(position);

}