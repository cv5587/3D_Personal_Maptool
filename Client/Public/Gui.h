#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CGui final : public CBase
{
private:
	CGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGui()=default;	

public:
	HRESULT Initialize();
	static CGui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Update_UI();
	HRESULT LateUpdate_UI();
	HRESULT Render();


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr }; 
	class CTerrainManager* m_pTerrainManager = { nullptr };

private:
	_bool		m_bInputObject = { false };
	_bool		m_bMakeObject = { false };
	_tchar		m_szRealFullPath[MAX_PATH] = TEXT("");
	wstring		m_ComponentTag;
private:
	_vector Picking_on_Terrain();
	_vector Picking_HitScreen();

public:
	virtual void Free() override;
};

END