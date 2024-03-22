#include "Model.h"

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Bone.h"

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
    , m_eModelType{ rhs.m_eModelType }
    , m_Bones{ rhs.m_Bones }
{
    //이건 얕은 복사라 문제 생김(동시 움직임)
    for (auto& pBone : m_Bones)
        Safe_AddRef(pBone);

    for (auto& MaterialDesc : m_Materials)
    {
        for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
            Safe_AddRef(MaterialDesc.MaterialTextures[i]);
    }

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    _uint		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    if (TYPE_NONANIM == eModelType)
        iFlag |= aiProcess_PreTransformVertices;

    /* assimp 열거체 aiProcess_PreTransformVertices
    메시들을 적당한뼈의 위치에 미리 다 배치시킨다.  정적물체는 괜찮지만 동적물체
    즉 애니메이션 값을 가진 모델이 이 열거값을 가지면 애니메이션값이 다 삭제됨*/
    m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    m_eModelType = eModelType;

    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    if (FAILED(Ready_Bones(m_pAIScene->mRootNode,-1)))
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

HRESULT CModel::Bind_BoneMatrices(CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex)
{
    ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 512);

    m_Meshes[iMeshIndex]->Fill_Matrices(m_Bones, m_MeshBoneMatrices);

    return pShaderCom->Bind_Matrices(pConstantName, m_MeshBoneMatrices, 512);
}

void CModel::Play_Animation(_float fTimeDelta)
{	
    /* 특정 애니메이션을 재생한다. == 특정 애니메이션에서 사용하는 뼈들의 TransformationMatrix를 갱신해준다. */

    /* 전체뼈를 순회하면서 모든 뼈의 CombinedTransformationMatrix를 갱신한다. */
    for (auto& pBone : m_Bones)
        pBone->Update_CombinedTransformationMatrix(m_Bones);
}

HRESULT CModel::Ready_Meshes()
{
    /* 하나의 모델은 여러개의 메시로 구성되어있다. */
    /* 메시 : 폴리곤의 집합. */
    /* 폴리곤 : 정점 세개. */

    /* -> 메시를 만든다 == 정점버퍼를 만든다. + 인덱스 */
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix), m_Bones);
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

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
    CBone* pBone = CBone::Create(pAINode, iParentIndex);
    if (nullptr == pBone)
        return E_FAIL;

    m_Bones.emplace_back(pBone);

    _uint iParent = m_Bones.size() - 1;

    for (_uint i = 0; i < pAINode->mNumChildren; ++i)
    {
        if (FAILED(Ready_Bones(pAINode->mChildren[i], iParent)))
            return E_FAIL;
    }

    return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PreTransformMatrix)))
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

    for (auto& pBone : m_Bones)
        Safe_Release(pBone);

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
