#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "BackGround.h"
#include "CUIObject.h"

#include "Shader.h"
#include "Texture.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC			EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowed = true;



	/* ���� �ʱ�ȭ������ ��ģ��. ( �׷��ȵ���̽� �ʱ�ȭ���� + �����Ŵ����� ����� �غ� �Ѵ�. ) */
	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;	

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	m_pGui->Create(m_pDevice,m_pContext);


	
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;
	

	return S_OK;
}

void CMainApp::Tick(float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);
	m_pGui->Update_UI();
}

HRESULT CMainApp::Render()
{

	m_pGui->Render();

	/* �׸���. */
	if (FAILED(m_pGameInstance->Draw(_float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;
	

	return S_OK;
}


HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Icon0"), CUIObject::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), 
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	/*For.Prototype_Component_Texture_Icon*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Asset2D/Textures/Icon/hinterlandLogo_texture.dds")))))
		return E_FAIL;

	/*For.Prototype_Component_Transform*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}




CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGui);


	/* ���۷��� ī��Ʈ�� 0���θ����. */
	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine();
}
