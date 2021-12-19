#include "Game/PhysicsWorld.h"
#include "Game/ContactCallBack.h"
#include "Model/Player.h"
#include <BulletDynamics\Character\btKinematicCharacterController.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

PhysicsWorld::PhysicsWorld() {
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(
		dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));        // �����������ٶ� Y����
}

/**
 * @brief ��ӽ�ɫ��������ҡ���ʬ��
 * @param orgin ��ʼλ��
 * @param index ��� 0��ʾ��� 1������Ϊ��ʬ
*/
void PhysicsWorld::addCharator(btVector3 orgin, int index) {
    btPairCachingGhostObject* ghostObject = new btPairCachingGhostObject();
    ghostObject->setUserIndex(index);

    // ������ײ��״
    btConvexShape* modelShape = new btCapsuleShape(0.5f, 0.1f);
    collisionShapes.push_back(modelShape);

    // �����任����
    btTransform modelTransform;
    modelTransform.setIdentity();
    modelTransform.setOrigin(orgin);

    ghostObject->setCollisionShape(modelShape);
    ghostObject->setWorldTransform(modelTransform);
    ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

    btKinematicCharacterController* character = new btKinematicCharacterController(ghostObject, modelShape, btScalar(0.5f));
    character->setGravity(btVector3(0, -10, 0));
    character->setStepHeight(btScalar(0.1f));
    character->setJumpSpeed(btScalar(1.1) * 3);

    dynamicsWorld->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter,
        btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::CharacterFilter);
    dynamicsWorld->addAction(character);

    m_character.push_back(character);
    m_ghostObject.push_back(ghostObject);
}

/**
 * @brief ��Ӿ�ֹ�����ĸ���
 * @param model �����ģ��
*/
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

    // ������ײ��״
    btCollisionShape* modelShape = new btBvhTriangleMeshShape(meshInterface, true);
    collisionShapes.push_back(modelShape);

    // �����任����
    btTransform modelTransform;
    modelTransform.setIdentity();
    modelTransform.setOrigin(btVector3(0, 0, 0));        // ����ԭ��λ��

    // ��������
    btScalar mass(0.0f);
    btVector3 localInertia(0, 0, 0);

    // �˶�״̬
    btDefaultMotionState* myMotionState = new btDefaultMotionState(modelTransform);
    // ���幹����Ϣ
    btRigidBody::btRigidBodyConstructionInfo  rbInfo(mass, myMotionState, modelShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    body->setUserIndex(0);

    // �������������̬������
    dynamicsWorld->addRigidBody(body);
}

/**
 * @brief ��ȡ����������ĳ��ʵ���ת������
 * @param index ʵ����±�
 * @return ת��
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
 * @brief ��������ģ��
*/
void PhysicsWorld::stepSimulation() {
    dynamicsWorld->stepSimulation(1.f / 150.f, 10);
}

/**
 * @brief ��ҹ��������߼��
*/
int PhysicsWorld::attackTest(Player* player) {
    glm::vec3 src = player->getPosition() + glm::vec3(0.0f, 0.5f, 0.0f);
    glm::vec3 dir = player->getCamera()->Front;
    btVector3 from(src.x, src.y, src.z);
    dir *= 1.1;
    src +=  dir;
    btVector3 to(src.x, src.y, src.z);
    btCollisionWorld::ClosestRayResultCallback callback(from, to);

    dynamicsWorld->rayTest(from, to, callback);
    if (callback.hasHit()) {
        const btCollisionObject* obj = callback.m_collisionObject;
        int index = obj->getUserIndex();

        if (index) {
            return index;
        }
    }
    return 0;
}

/**
 * @brief ��ұ������ļ��
 * @return ���ĸ���ʬ����
*/
int PhysicsWorld::attackedTest() {
    ContactCallBack callback = ContactCallBack();
    btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[1];
    dynamicsWorld->contactTest(obj, callback);

    return callback.u_attacked;
}

/**
 * @brief ����ĳ����ɫ
 * @param index 
*/
void PhysicsWorld::pushback(int index) {
    btKinematicCharacterController* charater = m_character[index];

    if (index > 0) {
        charater->jump();
        return;
    }
    
    btTransform& transform = m_ghostObject[index]->getWorldTransform();
    btVector3 forwardDir = transform.getBasis()[2];
    forwardDir = forwardDir.normalize();
    forwardDir *= 8;

    characterStop();
    charater->applyImpulse(-forwardDir);
    
}

/**
 * @brief ��ɫ��Ծ����
*/
void PhysicsWorld::characterJump() {
    if (m_character[0]->onGround()) {
        m_character[0]->jump();
    }
}

/**
 * @brief ��ɫ�ƶ�
 * @param direction �ƶ����� 
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
 * @brief ���½�ɫ����
 * @param yaw 
*/
void PhysicsWorld::updateCharacterFront(float yaw) {
    yaw -= 90.0f;
    m_ghostObject[0]->getWorldTransform().getBasis().setRotation(btQuaternion(btVector3(0, 1, 0), glm::radians(yaw)));
}

/**
 * @brief ��ɫֹͣ��·
*/
void PhysicsWorld::characterStop() {
    m_character[0]->setWalkDirection(btVector3(0.0, 0.0, 0.0));
}

void PhysicsWorld::respawn() {
    characterStop();
    m_ghostObject[0]->getWorldTransform().setOrigin(btVector3(0.0f, 50.0f, 0.0f));
}

PhysicsWorld::~PhysicsWorld() {
    //������ײ��״
    for (int j = 0; j < collisionShapes.size(); j++) {
        btCollisionShape* shape = collisionShapes[j];
        collisionShapes[j] = 0;
        delete shape;
    }

    // �����ɫ
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

    //����̬����
    delete dynamicsWorld;

    //���������
    delete solver;

    //����ֲ�׶�
    delete overlappingPairCache;

    //�������
    delete dispatcher;

    delete collisionConfiguration;
}