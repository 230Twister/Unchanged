#ifndef CONTACTCALLBACK
#define CONTACTCALLBACK

#include "btBulletCollisionCommon.h"

struct ContactCallBack : btCollisionWorld::ContactResultCallback {
    int u_attacked = 0;

    btScalar addSingleResult(
        btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0Wrap,
        int partId0,
        int index0,
        const btCollisionObjectWrapper* colObj1Wrap,
        int partId1,
        int index1) {
        
        int des0 = colObj0Wrap->getCollisionObject()->getUserIndex();
        int des1 = colObj1Wrap->getCollisionObject()->getUserIndex();
        u_attacked = 0;
        if (des1 >= 1) {
            u_attacked = des1;
        }
        else if (des0 >= 1) {
            u_attacked = des0;
        }

        return btScalar(0.f);
    };
};

#endif // !CONTACTCALLBACK
