#include "Game/PhysicsWorld.h"
#include <BulletDynamics\Character\btKinematicCharacterController.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

PhysicsWorld::PhysicsWorld() {
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(
		dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));        // 设置重力加速度 Y向下
}

void PhysicsWorld::addCharator(btVector3 orgin, int index) {
    btPairCachingGhostObject* ghostObject = new btPairCachingGhostObject();
    ghostObject->setUserIndex(index);

    // 建立碰撞形状
    btConvexShape* modelShape = new btCapsuleShape(0.5f, 0.1f);
    collisionShapes.push_back(modelShape);

    // 建立变换矩阵
    btTransform modelTransform;
    modelTransform.setIdentity();
    modelTransform.setOrigin(orgin);        // 设置原点位置

    ghostObject->setCollisionShape(modelShape);
    ghostObject->setWorldTransform(modelTransform);
    ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

    btKinematicCharacterController* character = new btKinematicCharacterController(ghostObject, modelShape, btScalar(0.5f));
    character->setGravity(btVector3(0, -10, 0));
    character->setStepHeight(btScalar(0.1f));
    character->setJumpSpeed(btScalar(1.1) * 3);

    dynamicsWorld->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter,
        btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    dynamicsWorld->addAction(character);

    m_character.push_back(character);
    m_ghostObject.push_back(ghostObject);
}

void PhysicsWorld::addRigidBody(Model* model) {
    btTriangleIndexVertexArray* meshInterface = new btTriangleIndexVertexArray();
    const vector<Mesh>& meshes = model->getMesh();
    int size = meshes.size();

    for (int i = 0; i < size; i++) {
        const Mesh& mesh = meshes[i];
        const vector<Vertex>& vertices = mesh.getVertices();
        const vector<unsigned int>& indices = mesh.getIndices();
        vector<btScalar>* btvertices = new vector<btScalar>;
        for (const Vertex& v : vertices) {
            btvertices->push_back(v.Position.x);
            btvertices->push_back(v.Position.y);
            btvertices->push_back(v.Position.z);
        }
        meshVertices.push_back(btvertices);

        btIndexedMesh part;

        part.m_vertexBase = (const unsigned char*)&(*btvertices)[0];
        part.m_vertexStride = sizeof(btScalar) * 3;
        part.m_numVertices = vertices.size();
        part.m_triangleIndexBase = (const unsigned char*)&indices[0];
        part.m_triangleIndexStride = sizeof(unsigned int) * 3;
        part.m_numTriangles = indices.size() / 3;
        part.m_indexType = PHY_INTEGER;

        meshInterface->addIndexedMesh(part, PHY_INTEGER);
    }

    // 建立碰撞形状
    btCollisionShape* modelShape = new btBvhTriangleMeshShape(meshInterface, true);
    collisionShapes.push_back(modelShape);

    // 建立变换矩阵
    btTransform modelTransform;
    modelTransform.setIdentity();
    modelTransform.setOrigin(btVector3(0, 0, 0));        // 设置原点位置

    btScalar mass(0.0f);
    btVector3 localInertia(0, 0, 0);        // 惯性

    // 运动状态
    btDefaultMotionState* myMotionState = new btDefaultMotionState(modelTransform);
    // 刚体构造信息
    btRigidBody::btRigidBodyConstructionInfo  rbInfo(mass, myMotionState, modelShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    // 将刚体添加至动态世界中
    dynamicsWorld->addRigidBody(body);
}

/**
 * @brief 获取物理世界中某个实体的转换矩阵
 * @param index 实体的下标
 * @return 转换
*/
btTransform& PhysicsWorld::getTransform(int index) {
    btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[index];
    btRigidBody* body = btRigidBody::upcast(obj);
    btTransform& trans = obj->getWorldTransform();
    if (body && body->getMotionState()) {
        body->getMotionState()->getWorldTransform(trans);
    }
    else {
        trans = obj->getWorldTransform();
    }
    return trans;
}

/**
 * @brief 物理世界模拟
*/
void PhysicsWorld::stepSimulation() {
    dynamicsWorld->stepSimulation(1.f / 150.f, 10);
}

/**
 * @brief 角色跳跃动作
*/
void PhysicsWorld::characterJump() {
    if (m_character[0]->onGround()) {
        m_character[0]->jump();
    }
}

/**
 * @brief 角色移动
 * @param direction 移动方向 
*/
void PhysicsWorld::characterWalk(WalkDirection direction, float deltaTime) {
    btTransform& transform = m_ghostObject[0]->getWorldTransform();
    btVector3 forwardDir = transform.getBasis()[2];

    btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
    btScalar walkVelocity = btScalar(1.1) * 8.0;        // 4 km/h -> 1.1 m/s
    btScalar walkSpeed = walkVelocity * deltaTime * 1.0f;

    if (direction == WalkDirection::RIGHT) {
        walkDirection += forwardDir;
        walkDirection = walkDirection.cross(btVector3(0.0, 1.0, 0.0));
    }
    else if (direction == WalkDirection::LEFT) {
        walkDirection += forwardDir;
        walkDirection = -walkDirection.cross(btVector3(0.0, 1.0, 0.0));
    }
    else if (direction == WalkDirection::UP) {
        walkDirection += forwardDir;
    }
    else if (direction == WalkDirection::DOWN) {
        walkDirection -= forwardDir;
    }

    m_character[0]->setWalkDirection(walkDirection * walkSpeed);
}

/**
 * @brief 更新角色朝向
 * @param yaw 
*/
void PhysicsWorld::updateCharacterFront(float yaw) {
    yaw -= 90.0f;
    m_ghostObject[0]->getWorldTransform().getBasis().setRotation(btQuaternion(btVector3(0, 1, 0), glm::radians(yaw)));
}

void PhysicsWorld::characterStop() {
    m_character[0]->setWalkDirection(btVector3(0.0, 0.0, 0.0));
}

PhysicsWorld::~PhysicsWorld() {
    //清理碰撞形状
    for (int j = 0; j < collisionShapes.size(); j++) {
        btCollisionShape* shape = collisionShapes[j];
        collisionShapes[j] = 0;
        delete shape;
    }

    // 清理角色
    for (btPairCachingGhostObject* e : m_ghostObject) {
        dynamicsWorld->removeCollisionObject(e);
    }
    for (btKinematicCharacterController* e : m_character) {
        dynamicsWorld->removeAction(e);
    }
    for (btPairCachingGhostObject* e : m_ghostObject) {
        delete e;
    }
    for (btKinematicCharacterController* e : m_character) {
        delete e;
    }
    
    for (std::vector<float>* vertices : meshVertices) {
        delete vertices;
    }

    //清理动态世界
    delete dynamicsWorld;

    //清理求解器
    delete solver;

    //清理粗测阶段
    delete overlappingPairCache;

    //清理调度
    delete dispatcher;

    delete collisionConfiguration;
}