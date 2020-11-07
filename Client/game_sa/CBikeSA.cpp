/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBikeSA.cpp
 *  PURPOSE:     Bike vehicle entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CBikeSAInterface::CalculateLeanMatrix()
{
    ((void(__thiscall*)(CBikeSAInterface*))0x6B7150)(this);
}

CBikeSA::CBikeSA(CBikeSAInterface* bike)
{
    DEBUG_TRACE("CBikeSA::CBikeSA( CBikeSAInterface * bike )");
    this->m_pInterface = bike;
}

CBikeSA::CBikeSA(eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2) : CVehicleSA(dwModelID, ucVariation, ucVariation2)
{
    DEBUG_TRACE("CBikeSA::CBikeSA( eVehicleTypes dwModelID ):CVehicleSA( dwModelID )");
    /*if(this->internalInterface)
    {
        // create the actual vehicle
        DWORD dwFunc = FUNC_CBikeContructor;
        DWORD dwThis = (DWORD)this->internalInterface;
        _asm
        {
            mov     ecx, dwThis
            push    MISSION_VEHICLE
            push    dwModelID
            call    dwFunc
        }

        this->SetEntityStatus(STATUS_ABANDONED); // so it actually shows up in the world

        pGame->GetWorld()->Add((CEntitySA *)this);
    }*/
}

CBikeHandlingEntry* CBikeSA::GetBikeHandlingData()
{
    return m_pBikeHandlingData;
}

void CBikeSA::SetBikeHandlingData(CBikeHandlingEntry* pBikeHandling)
{
    m_pBikeHandlingData = static_cast<CBikeHandlingEntrySA*>(pBikeHandling);
    GetBikeInterface()->m_pBikeHandlingData = m_pBikeHandlingData->GetInterface();
    RecalculateBikeHandling();
}

void CBikeSA::RecalculateBikeHandling()
{
    m_pBikeHandlingData->Recalculate();
}
