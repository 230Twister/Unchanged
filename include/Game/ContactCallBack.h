#ifndef CONTACTCALLBACK
#define CONTACTCALLBACK

#include "btBulletCollisionCommon.h"

struct ContactCallBack : btCollisionWorld::ContactResultCallback {
    int attacked = 0;

    btScalar addSingleResult(
        btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0Wrap,
        int partId0,
        int index0,
        const btCollisionObjectWrapper* colObj1Wrap,
        int partId1,
        int index1) {
        
        int des = colObj1Wrap->getCollisionObject()->getUserIndex();
        attacked = 0;
        if (des >= 1) {
            attacked = des;
        }

        return btScalar(0.f);
    };
};

#endif // !CONTACTCALLBACK
