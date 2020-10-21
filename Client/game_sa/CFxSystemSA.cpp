/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFxSystemSA.cpp
 *  PURPOSE:     Game effects handling
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

// Variables used in the hooks
static CFxSystemSAInterface*  ms_pUsingFxSystemSAInterface = NULL;
static float                  ms_fUsingDrawDistance = 0;
static ushort                 ms_usFxSystemSavedCullDistance = 0;
static CFxEmitterSAInterface* ms_pUsingFxEmitterSAInterface = NULL;
static ushort                 ms_usFxEmitterSavedFadeFarDistance = 0;
static ushort                 ms_usFxEmitterSavedFadeNearDistance = 0;
static float                  ms_fFxSystemUpdateCullDistMultiplier = FX_SYSTEM_UPDATE_CULL_DIST_MULTIPLIER_DEFAULT;
static float                  ms_fFxCreateParticleCullDistMultiplier = FX_CREATE_PARTICLE_CULL_DIST_MULTIPLIER_DEFAULT;

CFxSystemSAInterface::CFxSystemSAInterface()
{
    ((void(__thiscall*)(CFxSystemSAInterface*))0x4AAF00)(this);
}

CFxSystemSAInterface::~CFxSystemSAInterface()
{
    ((void(__thiscall*)(CFxSystemSAInterface*))0x4AA260)(this);
}

void CFxSystemSAInterface::Play()
{
    ((void(__thiscall*)(CFxSystemSAInterface*))0x4AA2F0)(this);
}

void CFxSystemSAInterface::Pause()
{
    ((void(__thiscall*)(CFxSystemSAInterface*))0x4AA370)(this);
}

void CFxSystemSAInterface::Stop()
{
    ((void(__thiscall*)(CFxSystemSAInterface*))0x4AA390)(this);
}

void CFxSystemSAInterface::PlayAndKill()
{
    ((void(__thiscall*)(CFxSystemSAInterface*))0x4AA3D0)(this);
}

void CFxSystemSAInterface::Kill()
{
    ((void(__thiscall*)(CFxSystemSAInterface*))0x4AA3F0)(this);
}

void CFxSystemSAInterface::AttachToBone(CEntitySAInterface* entity, int boneId)
{
    ((void(__thiscall*)(CFxSystemSAInterface*, CEntitySAInterface*, int))0x4AA400)(this, entity, boneId);
}

void CFxSystemSAInterface::AddParticle(RwV3d* position, RwV3d* velocity, float arg2, CFxPrtMult* prtMult, float arg4, float brightness, float arg6,
                                       unsigned char arg8)
{
    ((void(__thiscall*)(CFxSystemSAInterface*, RwV3d*, RwV3d*, float, CFxPrtMult*, float, float, float, unsigned char))0x4AA440)(
        this, position, velocity, arg2, prtMult, arg4, brightness, arg6, arg8);
}

void CFxSystemSAInterface::AddParticle(RwMatrixTag* transform, RwV3d* position, float arg2, CFxPrtMult* prtMult, float arg4, float arg5, float arg6,
                                       unsigned char arg7)
{
    ((void(__thiscall*)(CFxSystemSAInterface*, RwMatrixTag*, RwV3d*, float, CFxPrtMult*, float, float, float, unsigned char))0x4AA540)(
        this, transform, position, arg2, prtMult, arg4, arg5, arg6, arg7);
}

void CFxSystemSAInterface::SetConstTime(unsigned char arg0, float amount)
{
    ((void(__thiscall*)(CFxSystemSAInterface*, unsigned char, float))0x4AA6C0)(this, arg0, amount);
}

eFxSystemPlayStatus CFxSystemSAInterface::GetPlayStatus()
{
    return ((eFxSystemPlayStatus(__thiscall*)(CFxSystemSAInterface*))0x4AA900)(this);
}

void CFxSystemSAInterface::SetLocalParticles(unsigned char enable)
{
    ((void(__thiscall*)(CFxSystemSAInterface*, unsigned char))0x4AA910)(this, enable);
}

void CFxSystemSAInterface::SetOffsetPos(RwV3d* pos)
{
    ((void(__thiscall*)(CFxSystemSAInterface*, RwV3d*))0x4AA660)(this, pos);
}

CFxSystemSA::CFxSystemSA(CFxSystemSAInterface* pInterface)
{
    m_pInterface = pInterface;
    m_fDrawDistance = 0;
    pGame->GetFxManagerSA()->AddToList(m_pInterface, this);
}

CFxSystemSA::~CFxSystemSA()
{
    pGame->GetFxManagerSA()->RemoveFromList(this);
}

void CFxSystemSA::PlayAndKill()
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_FxSystem_c__PlayAndKill;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CFxSystemSA::GetMatrix(CMatrix& matrix)
{
    MemCpyFast(&matrix.vFront, &m_pInterface->matPosition.at, sizeof(CVector));
    MemCpyFast(&matrix.vPos, &m_pInterface->matPosition.pos, sizeof(CVector));
    MemCpyFast(&matrix.vUp, &m_pInterface->matPosition.up, sizeof(CVector));
    MemCpyFast(&matrix.vRight, &m_pInterface->matPosition.right, sizeof(CVector));
}

void CFxSystemSA::SetMatrix(const CMatrix& matrix)
{
    MemCpyFast(&m_pInterface->matPosition.at, &matrix.vFront, sizeof(CVector));
    MemCpyFast(&m_pInterface->matPosition.pos, &matrix.vPos, sizeof(CVector));
    MemCpyFast(&m_pInterface->matPosition.up, &matrix.vUp, sizeof(CVector));
    MemCpyFast(&m_pInterface->matPosition.right, &matrix.vRight, sizeof(CVector));
}

void CFxSystemSA::GetPosition(CVector& vecPos)
{
    vecPos.fX = m_pInterface->matPosition.pos.x;
    vecPos.fY = m_pInterface->matPosition.pos.y;
    vecPos.fZ = m_pInterface->matPosition.pos.z;
}

void CFxSystemSA::SetPosition(const CVector& vecPos)
{
    m_pInterface->matPosition.pos.x = vecPos.fX;
    m_pInterface->matPosition.pos.y = vecPos.fY;
    m_pInterface->matPosition.pos.z = vecPos.fZ;

    // Set the update flag(s)
    // this is what RwMatrixUpdate (@0x7F18E0) does
    m_pInterface->matPosition.flags &= 0xFFFDFFFC;
}

float CFxSystemSA::GetEffectDensity()
{
    return m_pInterface->sRateMult / 1000.0f;
}

void CFxSystemSA::SetEffectDensity(float fDensity)
{
    m_pInterface->sRateMult = (short)(fDensity * 1000.0f);
}

float CFxSystemSA::GetEffectSpeed()
{
    return m_pInterface->sTimeMult / 1000.0f;
}

void CFxSystemSA::SetEffectSpeed(float fSpeed)
{
    m_pInterface->sTimeMult = (short)(fSpeed * 1000.0f);
}

void CFxSystemSA::SetDrawDistance(float fDrawDistance)
{
    m_fDrawDistance = Clamp(0.f, fDrawDistance, 8191.f);
}

float CFxSystemSA::GetDrawDistance()
{
    if (HasCustomDrawDistance())
        return m_fDrawDistance;
    else
        return m_pInterface->pBlueprint->usCullDistance / 256.f;
}

bool CFxSystemSA::HasCustomDrawDistance()
{
    return m_fDrawDistance != 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// FxSystem_c::Update_MidA
//
// Poke new draw distance for the FxSystem
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) void OnMY_FxSystem_c_Update_MidA_Pre(CFxSystemSAInterface* pFxSystemSAInterface)
{
    CFxSystemSA* pFxSystemSA = pGame->GetFxManagerSA()->GetFxSystem(pFxSystemSAInterface);
    if (pFxSystemSA && pFxSystemSA->HasCustomDrawDistance())
    {
        ms_pUsingFxSystemSAInterface = pFxSystemSAInterface;
        ms_usFxSystemSavedCullDistance = pFxSystemSAInterface->pBlueprint->usCullDistance;
        ms_fUsingDrawDistance = pFxSystemSA->GetDrawDistance();
        pFxSystemSAInterface->pBlueprint->usCullDistance = (ushort)(ms_fUsingDrawDistance * 8);
        ms_fFxSystemUpdateCullDistMultiplier = 1 / 8.f;
    }
    else
        ms_fUsingDrawDistance = 0;
}

__declspec(noinline) void OnMY_FxSystem_c_Update_MidA_Post()
{
    if (ms_pUsingFxSystemSAInterface)
    {
        // Restore default settings
        ms_pUsingFxSystemSAInterface->pBlueprint->usCullDistance = ms_usFxSystemSavedCullDistance;
        ms_fUsingDrawDistance = 0;
        ms_pUsingFxSystemSAInterface = NULL;
        ms_fFxSystemUpdateCullDistMultiplier = FX_SYSTEM_UPDATE_CULL_DIST_MULTIPLIER_DEFAULT;
    }
}

// Hook info
#define HOOKCHECK_FxSystem_c_Update_MidA            0x83
#define HOOKPOS_FxSystem_c_Update_MidA              0x04AAF70
#define HOOKSIZE_FxSystem_c_Update_MidA             5
DWORD RETURN_FxSystem_c_Update_MidA = 0x04AAF75;
void _declspec(naked) HOOK_FxSystem_c_Update_MidA()
{
    _asm
    {
        pushad
        push    ecx
        call    OnMY_FxSystem_c_Update_MidA_Pre
        add     esp, 4*1
        popad

        push    [esp+4*2]
        push    [esp+4*2]
        call inner

        pushad
        call    OnMY_FxSystem_c_Update_MidA_Post
        popad
        retn    8

inner:
        sub     esp, 10h
        push    ebx
        push    ebp
        jmp     RETURN_FxSystem_c_Update_MidA
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// FxSystem_c::Update_MidB
//
// Poke new draw distances for each emitter
//
//////////////////////////////////////////////////////////////////////////////////////////
__declspec(noinline) void OnMY_FxSystem_c_Update_MidB_Pre(CFxEmitterSAInterface* pFxEmitterSAInterface)
{
    if (ms_fUsingDrawDistance)
    {
        dassert(pFxEmitterSAInterface->vtbl == (void*)0x085A7A4);
        ms_pUsingFxEmitterSAInterface = pFxEmitterSAInterface;

        ms_usFxEmitterSavedFadeFarDistance = pFxEmitterSAInterface->pBlueprint->usFadeFarDistance;
        ms_usFxEmitterSavedFadeNearDistance = pFxEmitterSAInterface->pBlueprint->usFadeNearDistance;

        pFxEmitterSAInterface->pBlueprint->usFadeFarDistance = (ushort)(ms_fUsingDrawDistance * 8);
        float fNearDistanceRatio = ms_usFxEmitterSavedFadeNearDistance / (float)ms_usFxEmitterSavedFadeFarDistance;
        pFxEmitterSAInterface->pBlueprint->usFadeNearDistance = (ushort)(ms_fUsingDrawDistance * fNearDistanceRatio * 8);
        ms_fFxCreateParticleCullDistMultiplier = 1 / 8.f;
    }
}

__declspec(noinline) void OnMY_FxSystem_c_Update_MidB_Post()
{
    if (ms_pUsingFxEmitterSAInterface)
    {
        // Restore default settings
        ms_pUsingFxEmitterSAInterface->pBlueprint->usFadeFarDistance = ms_usFxEmitterSavedFadeFarDistance;
        ms_pUsingFxEmitterSAInterface->pBlueprint->usFadeNearDistance = ms_usFxEmitterSavedFadeNearDistance;
        ms_pUsingFxEmitterSAInterface = NULL;
        ms_fFxCreateParticleCullDistMultiplier = FX_CREATE_PARTICLE_CULL_DIST_MULTIPLIER_DEFAULT;
    }
}

// Hook info
#define HOOKCHECK_FxSystem_c_Update_MidB           0x83
#define HOOKPOS_FxSystem_c_Update_MidB             0x04AB21D
#define HOOKSIZE_FxSystem_c_Update_MidB            7
DWORD RETURN_FxSystem_c_Update_MidB = 0x04AB224;
void _declspec(naked) HOOK_FxSystem_c_Update_MidB()
{
    _asm
    {
        pushad
        push    ecx
        call    OnMY_FxSystem_c_Update_MidB_Pre
        add     esp, 4*1
        popad

        mov     eax, [ebp+54h]
        push    eax
        call    dword ptr [edx+8]   // FxEmitter_c::CreateParticles

        pushad
        call    OnMY_FxSystem_c_Update_MidB_Post
        popad

        jmp     RETURN_FxSystem_c_Update_MidB
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CFxSystemSA::StaticSetHooks
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CFxSystemSA::StaticSetHooks()
{
    EZHookInstall(FxSystem_c_Update_MidA);
    EZHookInstall(FxSystem_c_Update_MidB);

    // Redirect these constants so we can change them
    MemPut<float*>(VAR_FxSystemUpdateCullDistMultiplier, &ms_fFxSystemUpdateCullDistMultiplier);
    MemPut<float*>(VAR_FxCreateParticleCullDistMultiplierA, &ms_fFxCreateParticleCullDistMultiplier);
    MemPut<float*>(VAR_FxCreateParticleCullDistMultiplierB, &ms_fFxCreateParticleCullDistMultiplier);
}
