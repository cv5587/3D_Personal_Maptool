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
	//m_pTerrainManager = CTerrainManager::Create();
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
		ImGui::SliderInt2("TerrainUV", iTerrain, 1, 1025);

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
		//m_pTerrainManager->Clone_Terrain(iTerrain);
		
		m_bInputObject = !m_bInputObject;
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
HRESULT CGui::Add_Components(void* pArg)
{
	
	return S_OK;
}

void CGui::Picking_on_Terrain()
{
	POINT ptmouse = {};
	GetCursorPos(&ptmouse);
	ScreenToClient(g_hWnd, &ptmouse);

	//뷰포트-투영-뷰스페이스-월드
	_vector	mousepos = {};
	mousepos.m128_f32[0] = ((2.0f * (_float)ptmouse.x) / (_float)g_iWinSizeX) - 1.0f;
	mousepos.m128_f32[1] = (((2.0f * (_float)ptmouse.y) / (_float)g_iWinSizeY) - 1.0f) * -1.0f;

}

void CGui::Free()
{
	//Safe_Release(m_pTerrainManager);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
