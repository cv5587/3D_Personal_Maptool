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

CGui::~CGui()
{

}

HRESULT CGui::Initialize()
{
	m_pCalculatorCom = CCalculator::Create(m_pDevice, m_pContext);
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
	if (ImGui::TreeNode("Object"))
	{

		{
			const char* items[] = { "CliffA" };
			static int item_current = 0;
			ImGui::ListBox("List", &item_current, items, IM_ARRAYSIZE(items));
			
		
		ImGui::NewLine();
			if (ImGui::Button("ObjectCreate") )
			{
				char szProtoname[MAX_PATH] = "Prototype_GameObject_";
				strcat_s(szProtoname, items[item_current]);

				MultiByteToWideChar(CP_ACP, 0, szProtoname, strlen(szProtoname), szRealFullPath, MAX_PATH);
				m_bMakeObject = true;
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

	if (m_bMakeObject &&(m_pGameInstance->Get_DIMouseState(DIM_LB) & 0x80))
	{
		CEnvironmentObject::ENVIRONMENT_DESC pDesc{};
		XMStoreFloat4(&pDesc.vPrePosition,Picking_on_Terrain());
		_float4 p = { 0.f,0.f,0.f,0.f };

		if( !Compare_Float4(p, pDesc.vPrePosition))
			{
				if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), szRealFullPath ,& pDesc) ))
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
_bool CGui::Compare_Float4(_float4 f1, _float4 f2)
{
	_vector v1 =XMLoadFloat4(&f1);
	_vector v2 =XMLoadFloat4(&f2);
	if (v1.m128_f32[0] != v2.m128_f32[0])
		return false;
	if (v1.m128_f32[1] != v2.m128_f32[1])
		return false;
	if (v1.m128_f32[2] != v2.m128_f32[2])
		return false;
	if (v1.m128_f32[3] != v2.m128_f32[3])
		return false;
	return true;
}
HRESULT CGui::Add_Components(void* pArg)
{
	
	return S_OK;
}

_vector CGui::Picking_on_Terrain()
{
	_matrix ViewMatrixInverse, ProjMatrixInverse;

	ViewMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);	
	ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TS_PROJ);

	POINT ptmouse = {};
	GetCursorPos(&ptmouse);
	ScreenToClient(g_hWnd, &ptmouse);


	//뷰포트-투영-뷰스페이스-월드
	_vector	mousepos = {};

	//투영으로 내림
	mousepos.m128_f32[0] = ((2.0f * (_float)ptmouse.x) / (_float)g_iWinSizeX) - 1.0f;	
	mousepos.m128_f32[1] = (1.0f - ((2.0f * (_float)ptmouse.y) / (_float)g_iWinSizeY) ) ;
	mousepos.m128_f32[2] = 0;


	//뷰스페이스로 내려줌
	mousepos=XMVector3TransformCoord(mousepos, ProjMatrixInverse);	

	_vector Raypos, Raydir;	
	Raypos = { 0,0,0 };
	Raydir = mousepos - Raypos;

	//월드로 내려줌
	Raypos =XMVector3TransformCoord(Raypos, ViewMatrixInverse);	
	Raydir =XMVector3TransformNormal(Raydir, ViewMatrixInverse);	


	//로컬로 내려줌
	_matrix TerrainWorldMatrixInverse;
	
	TerrainWorldMatrixInverse= XMMatrixInverse(nullptr, m_pTerrainManager->Get_Terrain_WorldMatrix());

	Raypos = XMVector3TransformCoord(Raypos, TerrainWorldMatrixInverse);	
	Raydir = XMVector3TransformNormal(Raydir, TerrainWorldMatrixInverse);
	Raydir=XMVector3Normalize(Raydir);

	_float4* pVtxPos=m_pTerrainManager->Get_Terrain_VtxPos();
	_int* pTerrainUV=m_pTerrainManager->Get_Terrain_UV();	


		_uint 	VtxIdx[3]{};
		float fDist(0.f);

	for (int i = 0; i < pTerrainUV[1] - 1; i++)
	{
		for (int j = 0; j < pTerrainUV[0] - 1; j++)
		{	
			_uint 	dwIndex = _uint(i * pTerrainUV[0] + j);

			VtxIdx[0] = dwIndex + (_uint)pTerrainUV[0];
			VtxIdx[1] = dwIndex + (_uint)pTerrainUV[0] + 1;
			VtxIdx[2] = dwIndex + 1;

			if (TriangleTests::Intersects(
				(_fvector)Raypos, (_fvector)Raydir, 
				(_fvector)XMLoadFloat4(&pVtxPos[VtxIdx[0]]), (_gvector)XMLoadFloat4(&pVtxPos[VtxIdx[1]]),
				(_hvector)XMLoadFloat4(&pVtxPos[VtxIdx[2]]), fDist)
				)
			{
				_vector d = {};
				d = (Raydir * fDist) + Raypos;
				return d;
			}
			// 왼쪽 아래
			VtxIdx[0] =  dwIndex + (_uint)pTerrainUV[0];
			VtxIdx[1] = dwIndex + 1;
			VtxIdx[2] = dwIndex;

			if (TriangleTests::Intersects(
				(_fvector)Raypos, (_fvector)Raydir,
				(_fvector)XMLoadFloat4(&pVtxPos[VtxIdx[0]]), (_gvector)XMLoadFloat4(&pVtxPos[VtxIdx[1]]),
				(_hvector)XMLoadFloat4(&pVtxPos[VtxIdx[2]]), fDist)
				)
			{				
				_vector d = {};
				d = (Raydir * fDist)+Raypos;
				return d;
			}

			
		}
	}

	return _vector{ 0.f,0.f,0.f,0.f };
}

void CGui::Free()
{
	Safe_Release(m_pTerrainManager);
	Safe_Release(m_pCalculatorCom);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
