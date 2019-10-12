/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPhysicsDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define MINIMUM_SHAPE_SIZE 0.05f

void CLuaPhysicsDefs::LoadFunctions(void)
{
    std::map<const char*, lua_CFunction> functions{
        {"physicsCreateWorld", PhysicsCreateWorld},
        {"physicsDestroy", PhysicsDestroy},
        {"physicsCreateRigidBody", PhysicsCreateRigidBody},
        {"physicsCreateRigidBodyFromModel", PhysicsCreateRigidBodyFromModel},
        {"physicsCreateStaticCollision", PhysicsCreateStaticCollision},
        {"physicsCreateShape", PhysicsCreateShape},
        {"physicsAddShape", PhysicsAddShape},
        {"physicsSetProperties", PhysicsSetProperties},
        {"physicsGetProperties", PhysicsGetProperties},
        {"physicsDrawDebug", PhysicsDrawDebug},
        {"physicsSetDebugMode", PhysicsSetDebugMode},
        {"physicsBuildCollisionFromGTA", PhysicsBuildCollisionFromGTA},
        {"physicsApplyForce", PhysicsApplyForce},
        {"physicsApplyCentralForce", PhysicsApplyCentralForce},
        {"physicsApplyCentralImpulse", PhysicsApplyCentralImpulse},
        {"physicsApplyDamping", PhysicsApplyDamping},
        {"physicsApplyImpulse", PhysicsApplyImpulse},
        {"physicsApplyTorque", PhysicsApplyTorque},
        {"physicsApplyTorqueImpulse", PhysicsApplyTorqueImpulse},
        {"physicsCreateConstraint", PhysicsCreateConstraint},
        {"physicsRayCast", PhysicsRayCast},
    };

    // Add functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
}

void CLuaPhysicsDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    // lua_classfunction(luaVM, "set", "PhysicsTestSet");
    // lua_classfunction(luaVM, "get", "PhysicsTestGet");

    lua_registerstaticclass(luaVM, "Physics");
}

int CLuaPhysicsDefs::PhysicsCreateWorld(lua_State* luaVM)
{
    CLuaPhysicsRigidBody*       pRigidBody = nullptr;
    CLuaPhysicsStaticCollision* pStaticCollision = nullptr;
    CLuaPhysicsConstraint*      pStaticConstraint = nullptr;

    ePhysicsProperty eProperty;
    CScriptArgReader argStream(luaVM);

    if (argStream.NextIsUserDataOfType<CLuaPhysicsRigidBody>())
        argStream.ReadUserData(pRigidBody);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsStaticCollision>())
        argStream.ReadUserData(pStaticCollision);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsConstraint>())
        argStream.ReadUserData(pStaticConstraint);

    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (luaMain)
    {
        CClientPhysics* pPhysics = new CClientPhysics(m_pManager, INVALID_ELEMENT_ID, luaMain);
        lua_pushelement(luaVM, pPhysics);
        return 1;
    }
    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaPhysicsDefs::PhysicsCreateShape(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    ePhysicsShapeType shapeType;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(shapeType);

    if (!argStream.HasErrors())
    {
        CLuaPhysicsShape*    pShape = pPhysics->CreateShape();
        CVector              vector;
        float                fRadius;
        float                fHeight;
        float                fHalf;
        float                fInitialChildCapacity;
        std::vector<CVector> vecList;
        switch (shapeType)
        {
            case PHYSICS_SHAPE_BOX:
                if (argStream.NextIsVector3D())
                {
                    argStream.ReadVector3D(vector);
                }
                else
                {
                    argStream.ReadNumber(fHalf);
                    vector = CVector(fHalf, fHalf, fHalf);
                }
                if (!argStream.HasErrors())
                {
                    if (vector.fX >= MINIMUM_SHAPE_SIZE && vector.fY >= MINIMUM_SHAPE_SIZE && vector.fZ >= MINIMUM_SHAPE_SIZE)
                    {
                        pShape->InitializeWithBox(vector);
                    }
                    else
                    {
                        argStream.SetCustomError(
                            SString("Minimum width, height and length must be equal or greater than %.02f units", MINIMUM_SHAPE_SIZE).c_str());
                    }
                }
                break;
            case PHYSICS_SHAPE_SPHERE:
                argStream.ReadNumber(fRadius);
                if (!argStream.HasErrors())
                {
                    if (fRadius >= MINIMUM_SHAPE_SIZE)
                    {
                        pShape->InitializeWithSphere(fRadius);
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Minimum radius must be equal or greater than %.02f units", MINIMUM_SHAPE_SIZE).c_str());
                    }
                }
                break;
            case PHYSICS_SHAPE_CAPSULE:
                argStream.ReadNumber(fRadius);
                argStream.ReadNumber(fHeight);
                if (!argStream.HasErrors())
                {
                    if (fRadius >= MINIMUM_SHAPE_SIZE && fHeight >= MINIMUM_SHAPE_SIZE)
                    {
                        pShape->InitializeWithCapsule(fRadius, fHeight);
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Minimum radius and height must be equal or greater than %.02f units", MINIMUM_SHAPE_SIZE).c_str());
                    }
                }
                break;
            case PHYSICS_SHAPE_CONE:
                argStream.ReadNumber(fRadius);
                argStream.ReadNumber(fHeight);
                if (!argStream.HasErrors())
                {
                    if (fRadius >= MINIMUM_SHAPE_SIZE && fHeight >= MINIMUM_SHAPE_SIZE)
                    {
                        pShape->InitializeWithCone(fRadius, fHeight);
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Minimum radius and height must be equal or greater than %.02f units", MINIMUM_SHAPE_SIZE).c_str());
                    }
                }
                break;
            case PHYSICS_SHAPE_CYLINDER:
                argStream.ReadVector3D(vector);
                if (!argStream.HasErrors())
                {
                    pShape->InitializeWithCylinder(vector);
                }
                break;
            case PHYSICS_SHAPE_COMPOUND:
                argStream.ReadNumber(fInitialChildCapacity, 0);
                if (!argStream.HasErrors())
                {
                    if (fInitialChildCapacity >= 0 && fInitialChildCapacity <= 1024)
                    {
                        pShape->InitializeWithCompound(fInitialChildCapacity);
                    }
                    else
                    {
                        argStream.SetCustomError("Initial capacity should be between 0 and 1024");
                    }
                }
                break;
            case PHYSICS_SHAPE_CONVEX_HULL:
                while (argStream.NextIsVector3D())
                {
                    argStream.ReadVector3D(vector);
                    vecList.push_back(vector);
                }
                if (!argStream.HasErrors())
                {
                    pShape->InitializeWithConvexHull(vecList);
                }
                break;
        }

        if (argStream.HasErrors())
        {
            pPhysics->DestroyShape(pShape);
        }
        else
        {
            lua_pushshape(luaVM, pShape);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsDestroy(lua_State* luaVM)
{
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (luaMain)
    {
        CClientPhysics* pPhysics = new CClientPhysics(m_pManager, INVALID_ELEMENT_ID, luaMain);
        lua_pushelement(luaVM, pPhysics);
        return 1;
    }
    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaPhysicsDefs::PhysicsBuildCollisionFromGTA(lua_State* luaVM)
{
    CClientPhysics*  pPhysics;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    if (!argStream.HasErrors())
    {
        pPhysics->BuildCollisionFromGTA();
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaPhysicsDefs::PhysicsSetDebugMode(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    ePhysicsDebugMode eDebugMode;
    bool              bEnabled;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(eDebugMode);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (pPhysics->SetDebugMode(eDebugMode, bEnabled))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
int CLuaPhysicsDefs::PhysicsDrawDebug(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    ePhysicsShapeType shapeType;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);

    if (!argStream.HasErrors())
    {
        pPhysics->DrawDebug();
    }
    return 1;
}

int CLuaPhysicsDefs::PhysicsCreateRigidBody(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    CLuaPhysicsShape* pShape;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadUserData(pShape);

    if (!argStream.HasErrors())
    {
        CLuaPhysicsRigidBody* pRigidBody = pPhysics->CreateRigidBody(pShape);
        lua_pushrigidbody(luaVM, pRigidBody);
        return 1;
    }
    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsCreateRigidBodyFromModel(lua_State* luaVM)
{
    CClientPhysics*  pPhysics;
    unsigned short   usModel;
    CVector          vecPosition;
    CVector          vecRotation;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadNumber(usModel);
    argStream.ReadVector3D(vecPosition, CVector(0, 0, 0));
    argStream.ReadVector3D(vecRotation, CVector(0, 0, 0));

    if (!argStream.HasErrors())
    {
        CLuaPhysicsRigidBody* pRigidBody = pPhysics->CreateRigidBodyFromModel(usModel, vecPosition, vecRotation);
        if (pRigidBody != nullptr)
            lua_pushrigidbody(luaVM, pRigidBody);
        else
            lua_pushboolean(luaVM, false);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsCreateStaticCollision(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    ePhysicsShapeType shapeType;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(shapeType);

    if (!argStream.HasErrors())
    {
        CLuaPhysicsStaticCollision* pStaticCollision = pPhysics->CreateStaticCollision();
        CVector                     vector;
        float                       fRadius;
        std::vector<CVector>        vecVector;
        int                         iSizeX, iSizeY;
        CVector                     vecScale;
        std::vector<float>          vecHeightfieldData;
        switch (shapeType)
        {
            case PHYSICS_SHAPE_BOX:
                argStream.ReadVector3D(vector);
                if (!argStream.HasErrors())
                {
                    pStaticCollision->InitializeWithBox(vector);
                }
                break;
            case PHYSICS_SHAPE_SPHERE:
                argStream.ReadNumber(fRadius);
                if (!argStream.HasErrors())
                {
                    pStaticCollision->InitializeWithSphere(fRadius);
                }
                break;
            case PHYSICS_SHAPE_TRIANGLE_MESH:
                while (argStream.NextIsVector3D())
                {
                    argStream.ReadVector3D(vector);
                    vecVector.push_back(vector);
                }
                if (!argStream.HasErrors())
                {
                    if (vecVector.size() % 3 == 0)
                    {
                        pStaticCollision->InitializeWithTriangleMesh(vecVector);
                    }
                }
                break;
            case PHYSICS_SHAPE_HEIGHTFIELD_TERRAIN:
                argStream.ReadNumber(iSizeX);
                argStream.ReadNumber(iSizeY);
                argStream.ReadVector3D(vecScale);
                argStream.ReadNumberTable(vecHeightfieldData, iSizeX * iSizeY);

                if (!argStream.HasErrors())
                {
                    pStaticCollision->InitializeWithHeightfieldTerrain(iSizeX, iSizeY, vecScale, vecHeightfieldData);
                }
                break;
        }
        lua_pushstaticcollision(luaVM, pStaticCollision);
        return 1;
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsAddShape(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    ePhysicsShapeType     shapeType;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadEnumString(shapeType);

    if (!argStream.HasErrors())
    {
        CVector vector;
        float   fRadius;
        switch (shapeType)
        {
            case PHYSICS_SHAPE_BOX:
                argStream.ReadVector3D(vector);
                if (!argStream.HasErrors())
                {
                    pRigidBody->AddBox(vector);
                }
                break;
            case PHYSICS_SHAPE_SPHERE:
                argStream.ReadNumber(fRadius);
                if (!argStream.HasErrors())
                {
                    pRigidBody->AddSphere(fRadius);
                }
                break;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyForce(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    CVector               from, to;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadVector3D(from);
    argStream.ReadVector3D(to);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyForce(from, to);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyCentralForce(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    CVector               force;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadVector3D(force);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyCentralForce(force);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyDamping(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    float                 fDamping;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadNumber(fDamping);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyDamping(fDamping);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyImpulse(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    CVector               from, to;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadVector3D(from);
    argStream.ReadVector3D(to);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyImpulse(from, to);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyTorque(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    CVector               force;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadVector3D(force);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyTorque(force);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyTorqueImpulse(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    CVector               force;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadVector3D(force);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyTorqueImpulse(force);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyCentralImpulse(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    CVector               force;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadVector3D(force);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyCentralImpulse(force);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsSetProperties(lua_State* luaVM)
{
    CLuaPhysicsRigidBody*       pRigidBody = nullptr;
    CLuaPhysicsStaticCollision* pStaticCollision = nullptr;
    CLuaPhysicsConstraint*      pStaticConstraint = nullptr;
    CLuaPhysicsShape*           pShape = nullptr;

    ePhysicsProperty eProperty;
    CScriptArgReader argStream(luaVM);

    if (argStream.NextIsUserDataOfType<CLuaPhysicsRigidBody>())
        argStream.ReadUserData(pRigidBody);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsStaticCollision>())
        argStream.ReadUserData(pStaticCollision);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsConstraint>())
        argStream.ReadUserData(pStaticConstraint);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsShape>())
        argStream.ReadUserData(pShape);

    argStream.ReadEnumString(eProperty);
    if (!argStream.HasErrors())
    {
        bool    boolean;
        CVector vector;
        float   floatNumber[2];
        int     intNumber;
        SColor  color;

        if (pRigidBody)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_STATIC:
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetStatic(boolean);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_MASS:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetMass(floatNumber[0]);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_POSITION:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetPosition(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_ROTATION:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetRotation(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                case PHYSICS_PROPERTY_SLEEPING_THRESHOLDS:
                    argStream.ReadNumber(floatNumber[0]);
                    argStream.ReadNumber(floatNumber[1]);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetSleepingThresholds(floatNumber[0], floatNumber[1]);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_RESTITUTION:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetRestitution(floatNumber[0]);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_SCALE:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetScale(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_DEBUG_COLOR:
                    argStream.ReadColor(color);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetDebugColor(color);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_FILTER_MASK:
                    argStream.ReadNumber(intNumber);
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        if (intNumber > 0 && intNumber < 32)
                        {
                            pRigidBody->SetFilterMask((char)intNumber, boolean);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_FILTER_GROUP:
                    argStream.ReadNumber(intNumber);
                    if (!argStream.HasErrors())
                    {
                        if (intNumber > 0 && intNumber <= 32)
                        {
                            pRigidBody->SetFilterGroup(intNumber);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                    }
                    break;
            }
        }
        else if (pStaticCollision)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_POSITION:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pStaticCollision->SetPosition(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_ROTATION:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pStaticCollision->SetRotation(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_SCALE:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pStaticCollision->SetScale(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_DEBUG_COLOR:
                    argStream.ReadColor(color);
                    if (!argStream.HasErrors())
                    {
                        pStaticCollision->SetDebugColor(color);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_FILTER_MASK:
                    argStream.ReadNumber(intNumber);
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        if (intNumber > 0 && intNumber < 32)
                        {
                            pStaticCollision->SetFilterMask((char)intNumber - 1, boolean);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_FILTER_GROUP:
                    argStream.ReadNumber(intNumber);
                    if (!argStream.HasErrors())
                    {
                        pStaticCollision->SetFilterGroup(intNumber);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
            }
        }
        else if (pStaticConstraint)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_STIFFNESS:
                    argStream.ReadNumber(intNumber);
                    argStream.ReadNumber(floatNumber[0]);
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        pStaticConstraint->SetStiffness(intNumber, floatNumber[0], boolean);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
            }
        }
        else if (pShape)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_SIZE:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        if (vector.fX >= MINIMUM_SHAPE_SIZE && vector.fY >= MINIMUM_SHAPE_SIZE && vector.fZ >= MINIMUM_SHAPE_SIZE)
                        {
                            if (pShape->SetSize(vector))
                            {
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                            else
                            {
                                argStream.SetCustomError(SString("Shape '%s' does not support size property", pShape->GetType()));
                            }
                        }
                        else
                        {
                            argStream.SetCustomError(
                                SString("Minimum width, height and length must be equal or greater than %.02f units", MINIMUM_SHAPE_SIZE).c_str());
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_SCALE:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        if (vector.fX >= MINIMUM_SHAPE_SIZE && vector.fY >= MINIMUM_SHAPE_SIZE && vector.fZ >= MINIMUM_SHAPE_SIZE)
                        {
                            if (pShape->SetScale(vector))
                            {
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                            else
                            {
                                argStream.SetCustomError(SString("Shape '%s' does not support scale property", pShape->GetType()));
                            }
                        }
                        else
                        {
                            argStream.SetCustomError(
                                SString("Minimum scale width, height and length must be equal or greater than %.02f units", MINIMUM_SHAPE_SIZE).c_str());
                        }
                    }
                    break;
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsGetProperties(lua_State* luaVM)
{
    CLuaPhysicsRigidBody*       pRigidBody = nullptr;
    CLuaPhysicsStaticCollision* pStaticCollision = nullptr;
    CLuaPhysicsConstraint*      pStaticConstraint = nullptr;
    CLuaPhysicsShape*           pShape = nullptr;

    ePhysicsProperty eProperty;
    CScriptArgReader argStream(luaVM);

    if (argStream.NextIsUserDataOfType<CLuaPhysicsRigidBody>())
        argStream.ReadUserData(pRigidBody);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsStaticCollision>())
        argStream.ReadUserData(pStaticCollision);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsConstraint>())
        argStream.ReadUserData(pStaticConstraint);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsShape>())
        argStream.ReadUserData(pShape);

    argStream.ReadEnumString(eProperty);
    if (!argStream.HasErrors())
    {
        bool    boolean;
        CVector vector, vector2;
        SColor  color;
        float   floatNumber[2];

        if (pRigidBody != nullptr)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_POSITION:
                    pRigidBody->GetPosition(vector);
                    lua_pushnumber(luaVM, vector.fX);
                    lua_pushnumber(luaVM, vector.fY);
                    lua_pushnumber(luaVM, vector.fZ);
                    return 3;
                case PHYSICS_PROPERTY_ROTATION:
                    pRigidBody->GetRotation(vector);
                    lua_pushnumber(luaVM, vector.fX);
                    lua_pushnumber(luaVM, vector.fY);
                    lua_pushnumber(luaVM, vector.fZ);
                    return 3;
                case PHYSICS_PROPERTY_SLEEPING_THRESHOLDS:
                    pRigidBody->GetSleepingThresholds(floatNumber[0], floatNumber[1]);
                    lua_pushnumber(luaVM, floatNumber[0]);
                    lua_pushnumber(luaVM, floatNumber[1]);
                    return 2;
                case PHYSICS_PROPERTY_RESTITUTION:
                    pRigidBody->GetRestitution(floatNumber[0]);
                    lua_pushnumber(luaVM, floatNumber[0]);
                    return 1;
                case PHYSICS_PROPERTY_SCALE:
                    pRigidBody->GetScale(vector);
                    lua_pushnumber(luaVM, vector.fX);
                    lua_pushnumber(luaVM, vector.fY);
                    lua_pushnumber(luaVM, vector.fZ);
                    return 3;
                case PHYSICS_PROPERTY_DEBUG_COLOR:
                    pRigidBody->GetDebugColor(color);
                    lua_pushnumber(luaVM, color.R);
                    lua_pushnumber(luaVM, color.G);
                    lua_pushnumber(luaVM, color.B);
                    return 3;
            }
        }
        else if (pStaticCollision)
        {
        }
        else if (pShape)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_SIZE:
                    if (pShape->GetSize(vector))
                    {
                        lua_pushnumber(luaVM, vector.fX);
                        lua_pushnumber(luaVM, vector.fY);
                        lua_pushnumber(luaVM, vector.fZ);
                        return 3;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Shape '%s' does not support size property", pShape->GetType()));
                    }
                    break;
                case PHYSICS_PROPERTY_BOUNDING_BOX:
                    if (pShape->GetBoundingBox(vector, vector2))
                    {
                        lua_pushnumber(luaVM, vector.fX);
                        lua_pushnumber(luaVM, vector.fY);
                        lua_pushnumber(luaVM, vector.fZ);
                        lua_pushnumber(luaVM, vector2.fX);
                        lua_pushnumber(luaVM, vector2.fY);
                        lua_pushnumber(luaVM, vector2.fZ);
                        return 6;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Shape '%s' does not support bounding box property", pShape->GetType()));
                    }
                    break;
                case PHYSICS_PROPERTY_BOUNDING_SPHERE:
                    if (pShape->GetBoundingSphere(vector, floatNumber[0]))
                    {
                        lua_pushnumber(luaVM, vector.fX);
                        lua_pushnumber(luaVM, vector.fY);
                        lua_pushnumber(luaVM, vector.fZ);
                        lua_pushnumber(luaVM, floatNumber[0]);
                        return 4;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Shape '%s' does not support bounding box property", pShape->GetType()));
                    }
                    break;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsCreateConstraint(lua_State* luaVM)
{
    CClientPhysics*       pPhysics;
    bool                  bDisableCollisionsBetweenLinkedBodies;
    CLuaPhysicsRigidBody* pRigidBodyA;
    CLuaPhysicsRigidBody* pRigidBodyB;
    ePhysicsConstraint    eConstraint;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(eConstraint);
    argStream.ReadBool(bDisableCollisionsBetweenLinkedBodies, true);
    argStream.ReadUserData(pRigidBodyA);
    argStream.ReadUserData(pRigidBodyB);

    if (!argStream.HasErrors())
    {
        CLuaPhysicsConstraint* pConstraint = pPhysics->CreateConstraint(pRigidBodyA, pRigidBodyB);
        CVector                vector[4];
        switch (eConstraint)
        {
            case PHYSICS_CONTRAINT_POINTTOPOINT:
                argStream.ReadVector3D(vector[0]);
                argStream.ReadVector3D(vector[1]);
                pConstraint->CreatePointToPointConstraint(vector[0], vector[1], bDisableCollisionsBetweenLinkedBodies);
                break;
            case PHYSICS_CONTRAINT_HIDGE:
                argStream.ReadVector3D(vector[0]);
                argStream.ReadVector3D(vector[1]);
                argStream.ReadVector3D(vector[2]);
                argStream.ReadVector3D(vector[3]);
                pConstraint->CreateHidgeConstraint(vector[0], vector[1], vector[2], vector[3], bDisableCollisionsBetweenLinkedBodies);
                break;
            case PHYSICS_CONTRAINT_FIXED:
                argStream.ReadVector3D(vector[0]);
                argStream.ReadVector3D(vector[1]);
                argStream.ReadVector3D(vector[2]);
                argStream.ReadVector3D(vector[3]);
                pConstraint->CreateFixedConstraint(vector[0], vector[1], vector[2], vector[3], bDisableCollisionsBetweenLinkedBodies);
                break;
            case PHYSICS_CONTRAINT_SLIDER:
                argStream.ReadVector3D(vector[0]);
                argStream.ReadVector3D(vector[1]);
                argStream.ReadVector3D(vector[2]);
                argStream.ReadVector3D(vector[3]);
                pConstraint->CreateSliderConstraint(vector[0], vector[1], vector[2], vector[3], bDisableCollisionsBetweenLinkedBodies);
                break;
        }
        lua_pushconstraint(luaVM, pConstraint);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsRayCast(lua_State* luaVM)
{
    CClientPhysics*  pPhysics;
    ePhysicsRayType  eRayType;
    CVector          from;
    CVector          to;
    bool             bFilterBackfaces;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(eRayType, PHYSICS_RAY_DEFAULT);
    argStream.ReadVector3D(from);
    argStream.ReadVector3D(to);
    argStream.ReadBool(bFilterBackfaces, false);

    if (!argStream.HasErrors())
    {
        if (eRayType == PHYSICS_RAY_IS_CLEAR)
        {
            bool bHasHit = pPhysics->RayCastIsClear(from, to);
            lua_pushboolean(luaVM, bHasHit);
            return 1;
        }
        else if (eRayType == PHYSICS_RAY_DEFAULT)
        {
            btCollisionWorld::ClosestRayResultCallback rayResult = pPhysics->RayCastDefault(from, to, bFilterBackfaces);
            lua_newtable(luaVM);
            lua_pushstring(luaVM, "hit");
            lua_pushboolean(luaVM, rayResult.hasHit());
            lua_settable(luaVM, -3);
            if (rayResult.hasHit())
            {
                lua_pushstring(luaVM, "hitpoint");
                lua_newtable(luaVM);
                lua_pushnumber(luaVM, 1);
                lua_pushnumber(luaVM, rayResult.m_hitPointWorld.getX());
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 2);
                lua_pushnumber(luaVM, rayResult.m_hitPointWorld.getY());
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 3);
                lua_pushnumber(luaVM, rayResult.m_hitPointWorld.getZ());
                lua_settable(luaVM, -3);
                lua_settable(luaVM, -3);

                lua_pushstring(luaVM, "hitnormal");
                lua_newtable(luaVM);
                lua_pushnumber(luaVM, 1);
                lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getX());
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 2);
                lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getY());
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 3);
                lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getZ());
                lua_settable(luaVM, -3);
            }
            lua_settable(luaVM, -3);
            return 1;
        }
        else if (eRayType == PHYSICS_RAY_DETAILED)
        {
            lua_newtable(luaVM);
            btCollisionWorld::ClosestRayResultCallback rayResult = pPhysics->RayCastDetailed(luaVM, from, to, bFilterBackfaces);
            lua_pushstring(luaVM, "hit");
            lua_pushboolean(luaVM, rayResult.hasHit());
            lua_settable(luaVM, -3);
            if (rayResult.hasHit())
            {
                lua_pushstring(luaVM, "hitpoint");
                lua_newtable(luaVM);
                lua_pushnumber(luaVM, 1);
                lua_pushnumber(luaVM, rayResult.m_hitPointWorld.getX());
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 2);
                lua_pushnumber(luaVM, rayResult.m_hitPointWorld.getY());
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 3);
                lua_pushnumber(luaVM, rayResult.m_hitPointWorld.getZ());
                lua_settable(luaVM, -3);
                lua_settable(luaVM, -3);

                lua_pushstring(luaVM, "hitnormal");
                lua_newtable(luaVM);
                lua_pushnumber(luaVM, 1);
                lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getX());
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 2);
                lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getY());
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 3);
                lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getZ());
                lua_settable(luaVM, -3);
                lua_settable(luaVM, -3);

                lua_pushstring(luaVM, "hitnormal");
                lua_newtable(luaVM);
                lua_pushnumber(luaVM, 1);
                lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getX());
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 2);
                lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getY());
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 3);
                lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getZ());
                lua_settable(luaVM, -3);
            }
            lua_settable(luaVM, -3);
            return 1;
        }
        else if (eRayType == PHYSICS_RAY_MULTIPLE)
        {
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}