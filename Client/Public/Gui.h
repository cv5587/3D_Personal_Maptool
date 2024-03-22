#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CGui final : public CBase
{
private:
	CGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGui();

public:
	HRESULT Initialize();
	static CGui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Update_UI();
	HRESULT LateUpdate_UI();
	HRESULT Render();


private:
	class CGameInstance* m_pGameInstance = { nullptr }; 
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	
	class CTerrainManager* m_pTerrainManager = { nullptr };
	class CCalculator* m_pCalculatorCom = { nullptr };

private:
	_bool m_bInputObject = { false };
	_bool	m_bTerrain = { false };




public:
	HRESULT Add_Components(void* pArg);
	_vector Picking_on_Terrain();
public:

	virtual void Free() override;
};

END