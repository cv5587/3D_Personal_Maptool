#include "Model.h"

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CComponent{pDevice,pContext}
{
}

CModel::CModel(const CModel& rhs)
    :CComponent{rhs}
    ,m_iNumMeshes{rhs.m_iNumMeshes}
    ,m_Meshes{rhs.m_Meshes}
    ,m_iNumMaterials{rhs.m_iNumMaterials}
    ,m_Materials{rhs.m_Materials }
{
    for (auto& MaterialDesc : m_Materials)
    {
        for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
            Safe_AddRef(MaterialDesc.MaterialTextures[i]);
    }

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);
}

HRESULT CModel::Initialize_Prototype(const _char* pModelFilePath)
{
    /* assimp ����ü aiProcess_PreTransformVertices
    �޽õ��� �����ѻ��� ��ġ�� �̸� �� ��ġ��Ų��.  ������ü�� �������� ������ü
    �� �ִϸ��̼� ���� ���� ���� �� ���Ű��� ������ �ִϸ��̼ǰ��� �� ������*/
    m_pAIScene = m_Importer.ReadFile(pModelFilePath, aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    if (FAILED(Ready_Meshes()))
        return E_FAIL;

    if (FAILED(Ready_Materials(pModelFilePath)))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
    m_Meshes[iMeshIndex]->Bind_Buffers();
    m_Meshes[iMeshIndex]->Render();

    return S_OK;
}

HRESULT CModel::Bind_Material(CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType)
{
    return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].MaterialTextures[eMaterialType]->Bind_ShaderResource(pShaderCom, pConstantName, 0);

}

HRESULT CModel::Ready_Meshes()
{

    /* �ϳ��� ���� �������� �޽÷� �����Ǿ��ִ�. */
    /* �޽� : �������� ����. */
    /* ������ : ���� ����. */

    /* -> �޽ø� ����� == �������۸� �����. + �ε��� */
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i]);
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.emplace_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
    m_iNumMaterials = m_pAIScene->mNumMaterials;

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

        MESH_MATERIAL		MeshMaterial{};

        for (size_t j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; j++)
        {
            aiString		strTextureFilePath;
            //pAIMaterial->GetTextureCount();
            if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTextureFilePath)))
                continue;

            _char		szFileName[MAX_PATH] = "";
            _char		szExt[MAX_PATH] = "";

            _splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            _char		szDrive[MAX_PATH] = "";
            _char		szDirectory[MAX_PATH] = "";

            _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

            _char		szFullPath[MAX_PATH] = "";

            strcpy_s(szFullPath, szDrive);
            strcat_s(szFullPath, szDirectory);
            strcat_s(szFullPath, szFileName);
            strcat_s(szFullPath, szExt);

            _tchar		szRealFullPath[MAX_PATH] = TEXT("");

            MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szRealFullPath, MAX_PATH);

            MeshMaterial.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szRealFullPath, 1);
            if (nullptr == MeshMaterial.MaterialTextures[j])
                return E_FAIL;
        }

        m_Materials.emplace_back(MeshMaterial);
    }

    return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pModelFilePath)))
    {
        MSG_BOX("Failed To Created : CModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
    CModel* pInstance = new CModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModel::Free()
{
    __super::Free();

    for (auto& MaterialDesc : m_Materials)
    {
        for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
            Safe_Release(MaterialDesc.MaterialTextures[i]);
    }

    m_Materials.clear();

    for (auto& pMesh : m_Meshes)
        Safe_Release(pMesh);

    m_Meshes.clear();

    m_Importer.FreeScene();
}
