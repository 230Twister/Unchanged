#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include <bullet/btBulletDynamicsCommon.h>
#include "Model/Model.h"

class btPairCachingGhostObject;
class btKinematicCharacterController;

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

	btKinematicCharacterController* character;
	btPairCachingGhostObject* ghostObject;

	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	std::vector<std::vector<float>*> meshVertices;
public:
	PhysicsWorld();
	void addCharator(Model*, btVector3);
	void addRigidBody(Model*);

	void characterJump();
	void characterWalk(WalkDirection, float);
	void updateCharacterFront(float);
	void characterStop();

	void stepSimulation();

	btTransform& getTransform(int);
	~PhysicsWorld();
};

#endif // !PHYSICSWORLD_H
