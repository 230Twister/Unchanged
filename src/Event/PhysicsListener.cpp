#include "Event/Event.h"
#include "Event/HandleList.h"
#include "Game/PhysicsWorld.h"

void PhysicsListener::setEvent(Event* event) {
	physicsEvent = static_cast<PhysicsEvent*>(event);
}

/**
 * @brief 同步物理世界与渲染世界
*/
void PhysicsListener::handle() {
	PhysicsWorld* physics = physicsEvent->getPhysicsWorld();
	World* world = physicsEvent->getWorld();

	btTransform& transform = physics->getTransform(1);
	btVector3& pos = transform.getOrigin();
	glm::vec3 position = glm::vec3(pos.getX(), pos.getY(), pos.getZ());
	
	Player* player = world->getPlayer();
	player->setPosition(position);
	player->setYaw(-player->getCamera()->Yaw);
	player->getCamera()->Position = glm::vec3(pos.getX(), pos.getY() + 2.0f, pos.getZ());
}