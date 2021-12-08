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
	std::vector<std::vector<float>*> meshVertices;
public:
	PhysicsWorld();
	void addMoveingRigidBody(Model*, btScalar, btVector3);
	void addRigidBody(Model*);
	void stepSimulation() { dynamicsWorld->stepSimulation(1.f / 150.f, 10); }

	btTransform getTrans() {
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[1];
		btRigidBody* body = btRigidBody::upcast(obj);
		btTransform trans;
		if (body && body->getMotionState())
		{
			body->getMotionState()->getWorldTransform(trans);
		}
		else
		{
			trans = obj->getWorldTransform();
		}
		return trans;
	}
	~PhysicsWorld();
};

#endif // !PHYSICSWORLD_H
