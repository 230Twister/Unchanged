#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include <bullet/btBulletDynamicsCommon.h>
#include "Model/Model.h"
#include "Model/Zombie.h"

class btPairCachingGhostObject;
class btKinematicCharacterController;
class Player;

enum class WalkDirection {
	RIGHT, LEFT, UP, DOWN
};

class PhysicsWorld {
private:
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	std::vector<btKinematicCharacterController*> m_character;
	std::vector<btPairCachingGhostObject*> m_ghostObject;

	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	std::vector<std::vector<float>*> meshVertices;
public:
	PhysicsWorld();
	void addCharator(btVector3, int);
	void addRigidBody(Model*);
	void characterJump();
	void characterWalk(WalkDirection, float);
	void updateCharacterFront(float);
	void characterStop();

	void stepSimulation();
	int attackTest(Player*);
	int attackedTest();
	void pushback(int);

	btTransform& getTransform(int);
	~PhysicsWorld();
};

#endif // !PHYSICSWORLD_H
