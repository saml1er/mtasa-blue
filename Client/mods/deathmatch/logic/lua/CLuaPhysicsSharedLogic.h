#include "bulletphysics3d/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#define MINIMUM_SHAPE_SIZE 0.003f            // to small collisions are not recommended

struct heightfieldTerrainShape
{
    btHeightfieldTerrainShape* pHeightfieldTerrainShape;
    std::vector<float>         data;
};

class CLuaPhysicsSharedLogic
    {
public:
    static void EulerToQuat(btVector3 rotation, btQuaternion& result);

    static bool SetPosition(btTransform& transform, CVector& vecRotation);
    static bool SetRotation(btTransform& transform, CVector& vecRotation);

    static bool SetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition);
    static bool SetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation);

    static bool SetScale(btCollisionShape* pCollisionShape, CVector& vecScale);
    static bool GetScale(btCollisionShape* pCollisionShape, CVector& vecScale);

    static btBoxShape*             CreateBox(CVector& half, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static btSphereShape*          CreateSphere(float fRadius, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static btCapsuleShape*         CreateCapsule(float fRadius, float fHeight);
    static btConeShape*            CreateCone(float fRadius, float fHeight);
    static btCylinderShape*        CreateCylinder(CVector& half);

    static btBvhTriangleMeshShape* CreateTriangleMesh(std::vector<CVector>& vecIndices);
    static heightfieldTerrainShape* CreateHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData);
    static btConvexHullShape*       CreateConvexHull(std::vector<CVector>& vecPoints);

    static btRigidBody* CreateRigidBody(btCollisionShape* pShape, float fMass);

    static bool AddBox(btCollisionObject* pCollisionObject, CVector& half, CVector& position = CVector(0, 0, 0), CVector& rotation = CVector(0, 0, 0));
    static bool AddBoxes(btCompoundShape* pCompoundShape, std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList);
    static bool AddBoxes(btCollisionObject* pCollisionObject, std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList);
    static bool AddSphere(btCollisionObject* pCollisionObject, float fRadius, CVector& position = CVector(0, 0, 0), CVector& rotation = CVector(0, 0, 0));
    static bool AddSpheres(btCollisionObject* pCollisionObject, std::vector<std::pair<float, std::pair<CVector, CVector>>>& spheresList);
    static bool AddTriangleMesh(btCollisionObject* pCollisionObject, std::vector<CVector>& vecIndices, CVector& position = CVector(0, 0, 0), CVector& rotation = CVector(0, 0, 0));

    static void QueryWorldObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static void QueryUserDefinedObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static CColModelSAInterface* GetModelCollisionInterface(ushort usModel);

    static void QuaternionToEuler(btQuaternion rotation, btVector3& result);
};
