#include "stdafx.h"
#include "Gui.h"
#include "GameInstance.h"

#include "TerrainManager.h"
#include "EnvironmentObject.h"


CGui::CGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	,m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CGui::Initialize()
{
	m_pTerrainManager = CTerrainManager::Create();
	return S_OK;
}

CGui* CGui::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGui* pInstance = new CGui(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Engine::Safe_Release(pInstance);
		MSG_BOX("ImGui Create Failed");
		return nullptr;
	}

	//ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);

	return pInstance;
}

//imgui 기능만 구현
HRESULT CGui::Update_UI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	bool bDemo = true;
	ImGui::ShowDemoWindow(&bDemo);



	static _int iTerrain[2] = { 0,0 };
	ImGui::Begin("MapTool");
	ImGui::SetWindowPos("MapTool", ImVec2(0, 0), ImGuiCond_Always);
	if (ImGui::TreeNode("MakeTerrain"))
	{
		ImGui::InputInt2("TerrainUV", iTerrain);
		ImGui::SliderInt2("TerrainUV", iTerrain, 3, 1025);

		if (ImGui::Button("Create")&& !m_bInputObject)
		{
			m_bInputObject = !m_bInputObject;
		}


		ImGui::NewLine();
		ImGui::TreePop();
	}



	const char* szObject = "EnvironmentObject";
	if (ImGui::TreeNode(szObject))
	{

		{
			const char* objects[] = { "CliffA" };
			static int object_current = 0;
			ImGui::ListBox("List", &object_current, objects, IM_ARRAYSIZE(objects));
			
		ImGui::NewLine();
			if (ImGui::Button("ObjectCreate") )
			{
				char szObjectname[MAX_PATH] = "Prototype_GameObject_";
				strcat_s(szObjectname, szObject);
				wcsset(m_szRealFullPath, L'\0');
				MultiByteToWideChar(CP_ACP, 0, szObjectname, strlen(szObjectname), m_szRealFullPath, MAX_PATH);
				
				char szComponentname[MAX_PATH] = "Prototype_Component_Model_";
				strcat_s(szComponentname, objects[object_current]);	
				wstring wstr(szComponentname, szComponentname + strlen(szComponentname));
				m_ComponentTag = wstr;

				m_bMakeObject = true;
			}

			if (ImGui::Button("ObjectBinary"))
			{
				char szProtoname[MAX_PATH] = "Prototype_Component_Model_";
				strcat_s(szProtoname, objects[object_current]);

				wstring wstr(szProtoname, szProtoname + strlen(szProtoname));
				wstring filepath = TEXT("../Bin/bin/");
				wstring Ext = TEXT(".bin");
				filepath=filepath + wstr+Ext;
				m_ComponentTag = wstr;
				if (FAILED(m_pGameInstance->Save_Binary(LEVEL_GAMEPLAY, wstr, filepath)))
					return E_FAIL;
			}
		}

		ImGui::NewLine();
		ImGui::TreePop();
	}



	const char* szMoster = "Monster";
	if (ImGui::TreeNode(szMoster))
	{

		{
			const char* Monsters[] = { "Fiona","Rabbit"};
			static int Monster_current = 0;
			ImGui::ListBox("List", &Monster_current, Monsters, IM_ARRAYSIZE(Monsters));


			ImGui::NewLine();
			if (ImGui::Button("MonsterCreate"))
			{

				char szMonstername[MAX_PATH] = "Prototype_GameObject_";
				strcat_s(szMonstername, szMoster);
				wcsset(m_szRealFullPath, L'\0');
				MultiByteToWideChar(CP_ACP, 0, szMonstername, strlen(szMonstername), m_szRealFullPath, MAX_PATH);

				char szComponentname[MAX_PATH] = "Prototype_Component_Model_";
				strcat_s(szComponentname, Monsters[Monster_current]);
				wstring wstr(szComponentname, szComponentname + strlen(szComponentname));
				m_ComponentTag = wstr;

				m_bMakeObject = true;
			}

			if (ImGui::Button("ObjectBinary"))
			{
				char szProtoname[MAX_PATH] = "Prototype_Component_Model_";
				strcat_s(szProtoname, Monsters[Monster_current]);

				wstring wstr(szProtoname, szProtoname + strlen(szProtoname));
				wstring filepath = TEXT("../Bin/bin/");
				wstring Ext = TEXT(".bin");
				filepath = filepath + wstr + Ext;
				m_ComponentTag = wstr;
				if (FAILED(m_pGameInstance->Save_Binary(LEVEL_GAMEPLAY, wstr, filepath)))
					return E_FAIL;
			}
		}

		ImGui::NewLine();
		ImGui::TreePop();
	}
	ImGui::End();
	if (0 == iTerrain[0] % 2)
		iTerrain[0]++;
	else if (0 == iTerrain[1] % 2)
		iTerrain[1]++;

	if (m_bInputObject)
	{

		m_pTerrainManager->Clone_Terrain(iTerrain);
		
		m_bInputObject = !m_bInputObject;
	}

	if (m_bMakeObject &&(m_pGameInstance->Get_DIMouseState(DIM_LB) & 0x80)&&(m_pGameInstance->Get_DIKeyState(DIK_LCONTROL) & 0x80))	
	{
		
		CGameObject::GAMEOBJECT_DESC pDesc{};

		XMStoreFloat4(&pDesc.vPrePosition, Picking_HitScreen());

		_float4 p = { -1.f,-1.f,-1.f,-1.f };

		pDesc.ComponentTag = m_ComponentTag;

		if( !m_pGameInstance->Compare_Float4(p, pDesc.vPrePosition))
			{
			//참고할 클래스,참고할 모델컴퍼넌트 테그 가져가기.
				if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), m_szRealFullPath,& pDesc) ))
					return E_FAIL;
		
				m_bMakeObject = false;
			}
	}



	return S_OK;
}

//연산
HRESULT CGui::LateUpdate_UI()
{
	return S_OK;
}

HRESULT CGui::Render()
{
	ImGui::Render();

	return S_OK;
}

_vector CGui::Picking_on_Terrain()
{
	_float4* pVtxPos = m_pTerrainManager->Get_Terrain_VtxPos();
	_int* pTerrainUV = m_pTerrainManager->Get_Terrain_UV();
	_float pWinSize[] = { g_iWinSizeX ,g_iWinSizeY };
	_matrix ViewMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);
	_matrix ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TS_PROJ);
	_matrix TerrainWorldMatrixInverse= XMMatrixInverse(nullptr, m_pTerrainManager->Get_Terrain_WorldMatrix());

	return m_pGameInstance->Picking_on_Terrain(g_hWnd, TerrainWorldMatrixInverse,
					ViewMatrixInverse, ProjMatrixInverse, pVtxPos, pTerrainUV, pWinSize);
}

_vector CGui::Picking_HitScreen()
{
	return m_pGameInstance->Picking_HitScreen();
}


void CGui::Free()
{
	Safe_Release(m_pTerrainManager);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
