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

	if (attackType < 0) {
		// 玩家受到攻击
		Player* player = world->getPlayer();
		Zombie* zombie = world->getZombie(-attackType - 1);

		if (!zombie->isDead()) {
			physics->pushback(0);
			if (player->setHealth(player->getHealth() - 5)) {
				physics->respawn();
			}
		}
		
	}
	else {
		// 僵尸受到攻击
		Zombie* zombie = world->getZombie(attackType - 1);

		if (!zombie->isDead()) {
			physics->pushback(attackType);
			zombie->setHealth(zombie->getHealth() - 15);
		}
	}
}
