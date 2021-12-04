#include "Game/PhysicsWorld.h"

PhysicsWorld::PhysicsWorld() {
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(
		dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));        // �����������ٶ� Y����
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

    btScalar mass(0.);                      // ����
    btVector3 localInertia(0, 0, 0);        // ����

    // �˶�״̬
    btDefaultMotionState* myMotionState = new btDefaultMotionState(modelTransform);
    // ���幹����Ϣ
    btRigidBody::btRigidBodyConstructionInfo  rbInfo(mass, myMotionState, modelShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    // �������������̬������
    dynamicsWorld->addRigidBody(body);
}

PhysicsWorld::~PhysicsWorld() {
    //������ײ��״
    for (int j = 0; j < collisionShapes.size(); j++) {
        btCollisionShape* shape = collisionShapes[j];
        collisionShapes[j] = 0;
        delete shape;
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