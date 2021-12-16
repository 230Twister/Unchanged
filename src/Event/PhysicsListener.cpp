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
	Camera* camera = player->getCamera();
	player->setPosition(position);
	player->setYaw(-camera->Yaw);
	if (player->getViewMode()) {
		camera->Position = glm::vec3(pos.getX() - 3.0f * cos(glm::radians(camera->Yaw)), pos.getY() + 1.8f, pos.getZ() - 3.0f * sin(glm::radians(camera->Yaw)));
	}
	else {
		camera->Position = glm::vec3(pos.getX(), pos.getY() + 0.45f , pos.getZ());
	}
}