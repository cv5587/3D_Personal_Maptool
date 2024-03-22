#include "EnvironmentObject.h"

#include "GameInstance.h"

EnvironmentObject::EnvironmentObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{pDevice,pContext}
{
}

EnvironmentObject::EnvironmentObject(const CGameObject& rhs)
    :CGameObject{rhs}
{
}

HRESULT EnvironmentObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT EnvironmentObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void EnvironmentObject::Priority_Tick(_float fTimeDelta)
{
}

void EnvironmentObject::Tick(_float fTimeDelta)
{

}

void EnvironmentObject::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT EnvironmentObject::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;



    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }


    return S_OK;
}

HRESULT EnvironmentObject::Add_Components()
{
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT EnvironmentObject::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
        return E_FAIL;


    return S_OK;
}

EnvironmentObject* EnvironmentObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    EnvironmentObject* pInstance = new EnvironmentObject(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : EnvironmentObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* EnvironmentObject::Clone(void* pArg)
{
    EnvironmentObject* pInstance = new EnvironmentObject(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : EnvironmentObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void EnvironmentObject::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
