#pragma once

#include "Base.h"

/* Ŭ���̾�Ʈ�����ڰ� ���� ����Ŭ�������� �θ� �Ǳ����� Ŭ�����̴�. */

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CGameInstance*		m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

END