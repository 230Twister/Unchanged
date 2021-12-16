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
	if (player->getViewMode()) {
		player->getCamera()->Position = glm::vec3(pos.getX() - 3.0f * cos(glm::radians(player->getCamera()->Yaw)), pos.getY() + 1.8f, pos.getZ() - 3.0f * sin(glm::radians(player->getCamera()->Yaw)));
	}
	else {
		// player->getCamera()->Position = glm::vec3(pos.getX() + 0.1f * sin(glm::radians(player->getCamera()->Yaw)), pos.getY() + 0.5f, pos.getZ() + 0.1f * sin(glm::radians(player->getCamera()->Yaw)));
		player->getCamera()->Position = glm::vec3(pos.getX(), pos.getY() + 0.45f , pos.getZ());
	}
}