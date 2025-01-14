#include "Data_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "TerrainManager.h"
#include "LandObject.h"	
#include "FreeCamera.h"
#include "Level_Loading.h"
#include "GEARStone.h"
CData_Manager::CData_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,CTerrainManager* pTerrainManager)
	:m_pGameInstance{CGameInstance::GetInstance()}
	,m_pTerrainMgr{pTerrainManager}
	, m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pGameInstance);	
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pTerrainMgr);
}

HRESULT CData_Manager::Load_Data(_uint iLevelIndex)
{

	char FileRoute[MAX_PATH] = "../Bin/bin/Save_Data/";
	char FilePath[MAX_PATH] = "";
	char FileDat[MAX_PATH] = ".dat";

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
	strcat_s(FileRoute, FileDat);


	
	ifstream fin;
	fin.open(FileRoute, ios::out | ios::binary);

	if (fin.is_open())
	{
		_uint iReadLevel=0;
		fin.read((char*)&iReadLevel, sizeof(_uint));

		m_pGameInstance->Clear_CloneData(iReadLevel);
		m_pTerrainMgr->Terrain_Release();

		_tchar Layer[MAX_PATH] = TEXT("");
		_tchar szProtoTag[MAX_PATH] = TEXT("");
		_tchar szModelTag[MAX_PATH] = TEXT("");
		_float4x4 fWorldPosition{};

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
				fin.read((char*)&fWorldPosition, sizeof(_float4x4));


				wstring wLayer(Layer);
				wstring strPrototypeTag(szProtoTag);
				wstring strModelTag(szModelTag);

				if (TEXT("Layer_BackGround") == wLayer)
				{
					_int iTerrain[2] = { 0,0 };
					fin.read((char*)iTerrain, sizeof(_int) * 2);

					m_pTerrainMgr->Clone_Terrain(iTerrain);
				}
				else if(TEXT("Layer_EnvironmentObject")==wLayer)
				{
					CGameObject::GAMEOBJECT_DESC pDesc{};
					pDesc.ProtoTypeTag = strPrototypeTag;
					pDesc.ModelTag = strModelTag;
					pDesc.vPrePosition = fWorldPosition;

					if (FAILED(m_pGameInstance->Add_CloneObject(iReadLevel, wLayer, strPrototypeTag, &pDesc)))
						return E_FAIL;
				}
				else if (TEXT("Layer_Player") == wLayer)
				{
					//암것도 하지마
				}
				else if (TEXT("Layer_Camera") == wLayer)
				{
					//암것도 하지마
				}
				else if (TEXT("Layer_Item") == wLayer)
				{
					CItem::ITEM_DESC ItemDesc{};
					
					ItemDesc.ProtoTypeTag = strPrototypeTag;
					ItemDesc.ModelTag = strModelTag;
					ItemDesc.vPrePosition = fWorldPosition;

					_tchar ItemName[MAX_PATH] = TEXT("");
					fin.read((char*)ItemName, sizeof(_tchar) * MAX_PATH);
					wstring wItemName(ItemName);

					ItemDesc.ItemName = wItemName;
					fin.read((char*)&ItemDesc.iQuantity, sizeof(_uint));
					fin.read((char*)&ItemDesc.ItemType[0], sizeof(_uint));
					fin.read((char*)&ItemDesc.ItemType[1], sizeof(_uint));

					if (FAILED(m_pGameInstance->Add_CloneObject(iReadLevel, wLayer, strPrototypeTag, &ItemDesc)))
						return E_FAIL;


				}
				else//몬스터 , 설정 (몬스터는 터레인만 추가,)
				{

					CLandObject::LANDOBJ_DESC		LandObjDesc{};

					LandObjDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
					LandObjDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
					LandObjDesc.ProtoTypeTag = strPrototypeTag;
					LandObjDesc.ModelTag = strModelTag;
					LandObjDesc.vPrePosition = fWorldPosition;

					if (FAILED(m_pGameInstance->Add_CloneObject(iReadLevel, wLayer, strPrototypeTag, &LandObjDesc)))
						return E_FAIL;
				}
			}
			
		}
	}

	fin.close();

	//툴에서는 카메라를 제작해주지만 인게임으로 넘어가서는 플레이어 생성시 카메라 자동생성으로 제작하기.
	CFreeCamera::FREE_CAMERA_DESC		CameraDesc{};

	CameraDesc.fSensor = 0.02f;
	CameraDesc.vEye = _float4(0.0f, 30.f, -25.f, 1.f);
	CameraDesc.vAt = _float4(0.0f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 50.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.f);
	XMStoreFloat4x4(&CameraDesc.vPrePosition, XMMatrixIdentity());
	CameraDesc.ProtoTypeTag = TEXT("Prototype_GameObject_FreeCamera");
	CameraDesc.ModelTag = TEXT("");
	XMStoreFloat4x4(&CameraDesc.vPrePosition, XMMatrixTranslation(0.f, 100.f, 0.f));
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &CameraDesc)))
		return E_FAIL;


	return S_OK;
}



HRESULT CData_Manager::Initialize()
{
	return S_OK;
}

CData_Manager* CData_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTerrainManager* pTerrainManager)
{
	CData_Manager* pInstance = new CData_Manager(pDevice, pContext, pTerrainManager);

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
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
