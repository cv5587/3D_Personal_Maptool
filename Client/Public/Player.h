#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

END

/* 플레이어를 구성하는 객체들을 들고 있는 객체이다. */

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum PART { PART_BODY, PART_WEAPON, PART_END };
	enum STATE { STATE_IDLE, STATE_RUN, STATE_ATTACK, STATE_END };

	static const _uint iState[STATE_END];
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;	

public:
	HRESULT Add_Components();	
	HRESULT Add_PartObjects();

private:
	vector<class CGameObject*>		m_PartObjects;
	_uint							m_iState = { 0 };

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END