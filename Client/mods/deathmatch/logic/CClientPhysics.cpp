/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPhysics.cpp
 *  PURPOSE:     Physics entity class
 *
 *****************************************************************************/

#include "StdInc.h"

void CDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
    m_pGraphics->DrawLine3DQueued(*(CVector*)&from, *(CVector*)&to, 4, color, false);
}
void CDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
    m_pGraphics->DrawLine3DQueued(*(CVector*)&from, *(CVector*)&to, 4, SColorARGB(255, lineColor.x() * 255.0f, lineColor.y() * 255.0f, lineColor.z() * 255.0f), false);
}
void CDebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& lineColor, btScalar alpha)
{
    m_pGraphics->DrawLine3DQueued(*(CVector*)&a, *(CVector*)&b, 4, color, false);
    m_pGraphics->DrawLine3DQueued(*(CVector*)&b, *(CVector*)&c, 4, color, false);
    m_pGraphics->DrawLine3DQueued(*(CVector*)&a, *(CVector*)&c, 4, color, false);
}

CClientPhysics::CClientPhysics(CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    // Init
    m_pManager = pManager;
    m_pPhysicsManager = pManager->GetPhysicsManager();

    SetTypeName("physics");

    m_pOverlappingPairCache = new btDbvtBroadphase();
    m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
    m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
    m_pSolver = new btSequentialImpulseConstraintSolver();
    m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pOverlappingPairCache, m_pSolver, m_pCollisionConfiguration);
    m_pDynamicsWorld->setGravity(btVector3(0, 0, -9.81f));

    m_pDebugDrawer = new CDebugDrawer(g_pCore->GetGraphics());
    m_pDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    m_pDynamicsWorld->setDebugDrawer(m_pDebugDrawer);

    // Add us to Physics manager's list
    m_pPhysicsManager->AddToList(this);
}

CClientPhysics::~CClientPhysics(void)
{
    // delete dynamics world
    delete m_pDynamicsWorld;
    delete m_pSolver;
    delete m_pOverlappingPairCache;
    delete m_pDispatcher;
    delete m_pCollisionConfiguration;

    // Remove us from Physics manager's list
    m_pPhysicsManager->RemoveFromList(this);
}

CLuaPhysicsRigidBody* CClientPhysics::CreateRigidBody(CLuaMain* luaMain)
{
    CLuaPhysicsRigidBody* pRigidBody = luaMain->GetPhysicsRigidBodyManager()->AddRigidBody(m_pDynamicsWorld);
    return pRigidBody;
}

CLuaPhysicsStaticCollision* CClientPhysics::CreateStaticCollision(CLuaMain* luaMain)
{
    CLuaPhysicsStaticCollision* pRigidBody = luaMain->GetPhysicsStaticCollisionManager()->AddStaticCollision(m_pDynamicsWorld);
    return pRigidBody;
}

bool CClientPhysics::SetDebugMode(ePhysicsDebugMode eDebugMode, bool bEnabled)
{
    if (eDebugMode == PHYSICS_DEBUG_NoDebug && bEnabled)
    {
        m_pDebugDrawer->setDebugMode(0);
        return true;
    }

    int debugMode = m_pDebugDrawer->getDebugMode();

    if (bEnabled)
    {
        debugMode |= 1UL << (int)eDebugMode;
    }
    else
    {
        debugMode &= ~(1UL << (int)eDebugMode);
    }
    m_pDebugDrawer->setDebugMode(debugMode);

    return true;
}

void CClientPhysics::DoPulse()
{
    CTickCount tickCountNow = CTickCount::Now();

    int iDeltaTimeMs = (int)(tickCountNow - m_LastTimeMs).ToLongLong();
    m_LastTimeMs = tickCountNow;

    m_pDynamicsWorld->stepSimulation(((float)iDeltaTimeMs) / 1000.0f);
    if (m_bDrawDebugNextTime)
    {
        m_pDynamicsWorld->debugDrawWorld();
        m_bDrawDebugNextTime = false;
    }
}
