#include "TerrainManager.h"
#include "Terrain.h"
#include "GameInstance.h"
CTerrainManager::CTerrainManager()	
	:m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CTerrainManager::Initialize()
{
	return S_OK;
}

HRESULT CTerrainManager::Clone_Terrain(void* pArg)	
{

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"), pArg,&m_pTerrain)))
		return E_FAIL;

	return S_OK;
}

CTerrainManager* CTerrainManager::Create()
{
	CTerrainManager* pInstance = new CTerrainManager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CTerrainManager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrainManager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pTerrain);
}
