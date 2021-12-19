#include "Event/Listener.h"
#include "Event/Event.h"
#include "Model/World.h"
#include "Game/PhysicsWorld.h"
#include "Model/Zombie.h"

void AttackListener::setEvent(Event* event) {
	attackEvent = static_cast<AttackEvent*>(event);
}

void AttackListener::handle() {
	int attackType = attackEvent->getAttackType();
	PhysicsWorld* physics = attackEvent->getPhysicsWorld();
	World* world = attackEvent->getWorld();

	if (attackType == 0) {
		// 玩家受到攻击
		physics->pushback(0);

		Player* player = world->getPlayer();
		player->setHealth(player->getHealth() - 5);
	}
	else {
		// 僵尸受到攻击
		physics->pushback(attackType);

		Zombie* zombie = world->getZombie(attackType - 1);
		zombie->setHealth(zombie->getHealth() - 5);
	}
}
