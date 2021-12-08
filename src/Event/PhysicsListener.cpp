#include "Event/Event.h"
#include "Event/HandleList.h"
#include "Game/PhysicsWorld.h"

void PhysicsListener::setEvent(Event* event) {
	physicsEvent = static_cast<PhysicsEvent*>(event);
}

void PhysicsListener::handle() {
	PhysicsWorld* physics = physicsEvent->getPhysicsWorld();
	World* world = physicsEvent->getWorld();

	btTransform transform = physics->getTrans();
	glm::vec3 position = glm::vec3(transform.getOrigin().getX() + 15, transform.getOrigin().getY(), transform.getOrigin().getZ());
	
	world->getPlayer()->setPosition(position);
}