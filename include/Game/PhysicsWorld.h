#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include <bullet/btBulletDynamicsCommon.h>
#include "Model/Model.h"

class PhysicsWorld {
private:
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	btAlignedObjectArray<btCollisionShape*> collisionShapes;
public:
	PhysicsWorld();
	void addRigidBody(Model* model);
	~PhysicsWorld();
};

#endif // !PHYSICSWORLD_H
