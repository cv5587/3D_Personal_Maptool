#include "Data_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "TerrainManager.h"

CData_Manager::CData_Manager(CTerrainManager* pTerrainManager)
	:m_pGameInstance{CGameInstance::GetInstance()}
	,m_pTerrainMgr{pTerrainManager}
{
	Safe_AddRef(m_pGameInstance);	
	Safe_AddRef(m_pTerrainMgr);
}

HRESULT CData_Manager::Load_Data(_uint iLevelIndex)
{

	char FileRoute[MAX_PATH] = "../Bin/bin/Save_Data/";
	char FilePath[MAX_PATH] = "";

	switch (iLevelIndex)
	{
	case 0:
		strcpy_s(FilePath, "LEVEL_STATIC_GAMEDATA");
		break;
	case 1:
		strcpy_s(FilePath, "LEVEL_LOADING_GAMEDATA");
		break;
	case 2:
		strcpy_s(FilePath, "LEVEL_LOGO_GAMEDATA");
		break;
	case 3:
		strcpy_s(FilePath, "LEVEL_GAMEPLAY_GAMEDATA");
		break;
	default:
		return E_FAIL;
	}

	strcat_s(FileRoute, FilePath);


	
	ifstream fin;
	fin.open(FileRoute, ios::out | ios::binary);

	if (fin.is_open())
	{
		_uint iReadLevel=0;
		fin.read((char*)&iReadLevel, sizeof(_uint));
		m_pGameInstance->Clear_CloneData(iReadLevel);

		_tchar Layer[MAX_PATH] = TEXT("");
		_tchar szProtoTag[MAX_PATH] = TEXT("");
		_tchar szModelTag[MAX_PATH] = TEXT("");
		_float4 fWorldPosition{};

		_uint Layersize =0;
		fin.read((char*)&Layersize, sizeof(_uint));
		for (_uint i = 0; i < Layersize; i++)	
		{

			fin.read((char*)&Layer, sizeof(_tchar) * MAX_PATH);
			
			_uint iObjectSize = 0;
			fin.read((char*)&iObjectSize, sizeof(_uint) );

			for (_uint i = 0; i < iObjectSize; i++)
			{
				fin.read((char*)szProtoTag, sizeof(_tchar) * MAX_PATH);
				fin.read((char*)szModelTag, sizeof(_tchar) * MAX_PATH);
				fin.read((char*)&fWorldPosition, sizeof(_float4));


				wstring wLayer(Layer);
				wstring strPrototypeTag(szProtoTag);
				wstring strModelTag(szModelTag);

				if (TEXT("Prototype_Component_VIBuffer_Terrain") == strModelTag)
				{
					_int iTerrain[2] = { 0,0 };
					fin.read((char*)iTerrain, sizeof(_int) * 2);

					m_pTerrainMgr->Clone_Terrain(iTerrain);
				}
				else
				{
					CGameObject::GAMEOBJECT_DESC pDesc{};
					pDesc.ProtoTypeTag = strPrototypeTag;
					pDesc.ModelTag = strModelTag;
					pDesc.vPrePosition = fWorldPosition;

					if (FAILED(m_pGameInstance->Add_CloneObject(iReadLevel, wLayer, strPrototypeTag, &pDesc)))
						return E_FAIL;
				}
			}
			
		}
	}

	fin.close();

	return S_OK;
}

HRESULT CData_Manager::Initialize()
{
	m_pTerrainMgr->Create();
	return S_OK;
}

CData_Manager* CData_Manager::Create(CTerrainManager* pTerrainManager)
{
	CData_Manager* pInstance = new CData_Manager(pTerrainManager);

	if (FAILED(pInstance->Initialize()))
	{
		Engine::Safe_Release(pInstance);
		MSG_BOX("Data_Manager Create Failed");
		return nullptr;
	}
	return pInstance;
}

void CData_Manager::Free()
{
	Safe_Release(m_pTerrainMgr);
	Safe_Release(m_pGameInstance);
}
