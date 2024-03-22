#include "stdafx.h"
#include "Gui.h"
#include "GameInstance.h"


#include "TerrainManager.h"


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
		ImGui::Checkbox("LockTerrain", &m_bTerrain);
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



		if (ImGui::Button("ObjectCreate") )
		{

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

	if (m_bTerrain&&(m_pGameInstance->Get_DIMouseState(DIM_LB) & 0x80))
		Picking_on_Terrain();
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

	_vector* pVtxPos=m_pTerrainManager->Get_Terrain_VtxPos();
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
				(_fvector)pVtxPos[VtxIdx[0]], (_gvector)pVtxPos[VtxIdx[1]], 
				(_hvector)pVtxPos[VtxIdx[2]], fDist)
				)
			{
				_vector d = {};
				d.m128_f32[0] = Raydir.m128_f32[0] * fDist;
				d.m128_f32[1] = Raydir.m128_f32[1] * fDist;
				d.m128_f32[2] = Raydir.m128_f32[2] * fDist;
				d.m128_f32[3] = 1.f;
				return d;
			}
			// 왼쪽 아래
			VtxIdx[0] =  dwIndex + (_uint)pTerrainUV[0];
			VtxIdx[1] = dwIndex + 1;
			VtxIdx[2] = dwIndex;

			if (TriangleTests::Intersects(
				(_fvector)Raypos, (_fvector)Raydir,
				(_fvector)pVtxPos[VtxIdx[0]], (_gvector)pVtxPos[VtxIdx[1]],
				(_hvector)pVtxPos[VtxIdx[2]], fDist)
				)
			{				
				_vector d = {};
				d.m128_f32[0]=Raydir.m128_f32[0]* fDist;
				d.m128_f32[1]=Raydir.m128_f32[1] * fDist;
				d.m128_f32[2]=Raydir.m128_f32[2]* fDist;
				d.m128_f32[3] = 1.f;
				return d;
			}

			
		}
	}

	return _vector{ 0,0,0 };
	//bool XM_CALLCONV Intersects(
//	FXMVECTOR Origin,
//	FXMVECTOR Direction,
//	FXMVECTOR V0,
//	GXMVECTOR V1,
//	HXMVECTOR V2,
//	float& Dist
//) noexcept;


	////터레인의 버텍스 정보는 로컬이다.
	//const _vec3* pTerrainVtxPos = pTerrainBufferCom->Get_VtxPos();

	//_ulong	dwVtxIdx[3]{};
	//_float	fU(0.f), fV(0.f), fDist(0.f);

	//for (_ulong i = 0; i < dwCntZ - 1; ++i)
	//{
	//	for (_ulong j = 0; j < dwCntX - 1; ++j)
	//	{
	//		_ulong	dwIndex = i * dwCntX + j;

	//		// 오른쪽 위

	//		dwVtxIdx[0] = dwIndex + dwCntX;
	//		dwVtxIdx[1] = dwIndex + dwCntX + 1;
	//		dwVtxIdx[2] = dwIndex + 1;

	//		if (D3DXIntersectTri(&pTerrainVtxPos[dwVtxIdx[1]],
	//			&pTerrainVtxPos[dwVtxIdx[0]],
	//			&pTerrainVtxPos[dwVtxIdx[2]],
	//			&vRayPos, &vRayDir,
	//			&fU, &fV, &fDist))
	//		{
	//			return _vec3(pTerrainVtxPos[dwVtxIdx[1]].x + fU * (pTerrainVtxPos[dwVtxIdx[0]].x - pTerrainVtxPos[dwVtxIdx[1]].x),
	//				0.f,
	//				pTerrainVtxPos[dwVtxIdx[1]].z + fV * (pTerrainVtxPos[dwVtxIdx[2]].z - pTerrainVtxPos[dwVtxIdx[0]].z));
	//		}

	//		// V1 + U(V2 - V1) + V(V3 - V1)

	//		// 왼쪽 아래
	//		dwVtxIdx[0] = dwIndex + dwCntX;
	//		dwVtxIdx[1] = dwIndex + 1;
	//		dwVtxIdx[2] = dwIndex;

	//		if (D3DXIntersectTri(&pTerrainVtxPos[dwVtxIdx[2]],
	//			&pTerrainVtxPos[dwVtxIdx[1]],
	//			&pTerrainVtxPos[dwVtxIdx[0]],
	//			&vRayPos, &vRayDir,
	//			&fU, &fV, &fDist))
	//		{
	//			return _vec3(pTerrainVtxPos[dwVtxIdx[2]].x + fU * (pTerrainVtxPos[dwVtxIdx[1]].x - pTerrainVtxPos[dwVtxIdx[2]].x),
	//				0.f,
	//				pTerrainVtxPos[dwVtxIdx[2]].z + fV * (pTerrainVtxPos[dwVtxIdx[0]].z - pTerrainVtxPos[dwVtxIdx[2]].z));
	//		}
	//	}
	//}
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
