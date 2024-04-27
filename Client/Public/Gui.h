#pragma once
#include "Client_Defines.h"
#include "Base.h"
BEGIN(Engine)
class CNavigation;	
END


BEGIN(Client)
class CGui final : public CBase
{
private:
	CGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGui()=default;	

public:
	HRESULT Initialize();
	static CGui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Update_UI(_float fTimeDelta);
	HRESULT LateUpdate_UI();
	HRESULT Render();


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr }; 
	class CTerrainManager* m_pTerrainManager = { nullptr };
	class CData_Manager* m_pData_Manager = { nullptr };

private:
	_bool		m_bInputObject = { false };
	_bool		m_bMakeObject = { false };
	_tchar		m_szRealFullPath[MAX_PATH] = TEXT("");
	_tchar		m_szLayerPath[MAX_PATH] = TEXT("");
	wstring		m_ComponentTag;
	class CGameObject* m_pPickObject = { nullptr };

	class CNavigation* m_pNavigationCom = nullptr;
private:
	_int m_iObjectID = { 0 };
	_bool m_CellMode = { false };
	_float3 m_Points[3] = {};
	_float m_SnapReach = { 1.f };
	_int		m_CellIndex = { 0 };
	_int* m_PlayerCell = { nullptr };
private:
	_vector Picking_HitScreen();
	void EditTransform(_float* cameraView, _float* cameraProjection, _float* matrix);
	void TestGuizmo(_float fTimeDelta);
	_matrix CameraStateChange(_fmatrix CamWorld,_cmatrix ObjWorld, _float fTimeDelta);
	
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag,
		class CComponent** ppOut, void* pArg = nullptr);
public:
	void Make_Terrain(void* pArg);
public:
	virtual void Free() override;
};

END