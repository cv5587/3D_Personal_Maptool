#pragma once
#include "State.h"

BEGIN(Client)
class CState_Jog final:
    public CState
{
protected:
    CState_Jog();
    virtual ~CState_Jog() = default;

public:
    virtual void Enter(class CPlayer* Player) override;
    virtual void Update(class CPlayer* Player, _float fTimeDelta)override;
    virtual void Exit(class CPlayer* Player)override;

public:
    static CState* Create();
    virtual void Free() override;
};

END