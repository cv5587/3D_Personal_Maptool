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
    , m_iNumMeshes{ rhs.m_iNumMeshes }
    , m_Meshes{ rhs.m_Meshes }
    , m_iNumMaterials{ rhs.m_iNumMaterials }
    , m_Materials{ rhs.m_Materials }
    , m_eModelType{ rhs.m_eModelType }
    /*, m_Bones{ rhs.m_Bones }*/
    , m_PreTransformMatrix{ rhs.m_PreTransformMatrix }
    , m_iNumAnimations{ rhs.m_iNumAnimations }
    /*, m_Animations{ rhs.m_Animations }	*/
{
    for (auto& pPrototypeAnimation : rhs.m_Animations)
        m_Animations.emplace_back(pPrototypeAnimation->Clone());

    for (auto& pPrototypeBone : rhs.m_Bones)
        m_Bones.emplace_back(pPrototypeBone->Clone());

    for (auto& MaterialDesc : m_Materials)
    {
        for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
            Safe_AddRef(MaterialDesc.MaterialTextures[i]);
    }

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);
}

const _float4x4* CModel::Get_BoneCombinedTransformationMatrix(const _char* pBoneName) const
{

	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
	{
		return pBone->Compare_Name(pBoneName);
	});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationMatrix();	
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    /*_uint		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;*/

    //if (TYPE_NONANIM == eModelType)
    //    iFlag |= aiProcess_PreTransformVertices;

    _uint		iFlag;
    if (TYPE_NONANIM == eModelType)
        iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
      //  iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace;
    else
        iFlag = aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace;
       // iFlag = aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace| aiProcess_ValidateDataStructure;
     

    /* assimp ����ü aiProcess_PreTransformVertices
    �޽õ��� �����ѻ��� ��ġ�� �̸� �� ��ġ��Ų��.  ������ü�� �������� ������ü
    �� �ִϸ��̼� ���� ���� ���� �� ���Ű��� ������ �ִϸ��̼ǰ��� �� ������*/
    m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    m_eModelType = eModelType;

    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    if (FAILED(Ready_Bones(m_pAIScene->mRootNode,-1)))
        return E_FAIL;

    if (FAILED(Ready_Materials(pModelFilePath)))
        return E_FAIL;

    if (FAILED(Ready_Meshes()))
        return E_FAIL;

    if (FAILED(Ready_Animations()))
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
    if (false == m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].isTextured)
        return S_OK;
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
    /* Ư�� �ִϸ��̼��� ����Ѵ�. == Ư�� �ִϸ��̼ǿ��� ����ϴ� ������ TransformationMatrix�� �������ش�. */
    /* ���� �ִϸ��̼��� ���¿� �µ��� ������ �������(TransformationMatrix)�� ����� �������ش�. */
    /* m_Animations[m_iCurrentAnimIndex] : �� �ִϸ��̼ǿ��� ��뤷�ϴ� ������ ���������� */
    m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_AnimDesc.isLoop);

    /* ��ü���� ��ȸ�ϸ鼭 ��� ���� CombinedTransformationMatrix�� �����Ѵ�. */
    for (auto& pBone : m_Bones)
        pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
}

HRESULT CModel::Make_Binary(const wstring FilePath)
{
    ofstream fout;
    fout.open(FilePath, ios::out    |   ios::binary);

    if (!fout.fail())
    {
        fout.write(reinterpret_cast<char*>(&m_PreTransformMatrix), sizeof(_float4x4));
        fout.write(reinterpret_cast<char*>(&m_AnimDesc), sizeof(ANIMATION_DESC));
       
        _uint iBoneSize = m_Bones.size();
        fout.write(reinterpret_cast<char*>(&iBoneSize), sizeof(_uint));

        for (auto& SaveBone : m_Bones)
            SaveBone->Save_Bone(&fout);

        fout.write(reinterpret_cast<char*>(&m_eModelType), sizeof(MODELTYPE));
        fout.write(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));

        for (auto& SaveMesh : m_Meshes)
            SaveMesh->Save_Meshes(&fout,m_eModelType);

        fout.write(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));
        for (auto& SaveMaterial : m_Materials)
        {
            fout.write(reinterpret_cast<char*>(&SaveMaterial.isTextured), sizeof(_bool));
            if(SaveMaterial.isTextured)
            {
                for (size_t i = 1; i < AI_TEXTURE_TYPE_MAX; i++)
                {
                    if (nullptr != SaveMaterial.MaterialTextures[i])
                    {
                        _uint check = 1;
                        fout.write(reinterpret_cast<char*>(&check), sizeof(_uint));
                        SaveMaterial.MaterialTextures[i]->Save_Texture(&fout);
                    }
                    else
                    {
                        _uint check = 0;
                        fout.write(reinterpret_cast<char*>(&check), sizeof(_uint));
                    }
                }
            }
        }



        //�ϴ� ���� ���� �ִ��� ���ִ��� ������ �ϴ� ���� ����������
        if(TYPE_ANIM== m_eModelType)
        {
            fout.write(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));

            for (auto& SaveAni : m_Animations)
                SaveAni->Save_Animation(&fout);
        }
        
    }
    fout.close();

    return S_OK;
}

HRESULT CModel::Read_Binary( char* FilePath)
{
    ifstream fin(FilePath, ios::binary);

    if(fin.is_open())
    { 
    //�θ�� �ε����� -1�� ���� �����ؼ� -2 �� �ϱ�
        fin.read(reinterpret_cast<char*>(&m_PreTransformMatrix), sizeof(_float4x4));
        fin.read(reinterpret_cast<char*>(&m_AnimDesc), sizeof(ANIMATION_DESC));

        _uint iBoneSize = 0;
        fin.read(reinterpret_cast<char*>(&iBoneSize), sizeof(_uint));
        for (size_t i = 0; i < iBoneSize; i++)
        {
            CBone* pBone = CBone::Create(&fin);
            if (nullptr == pBone)
                return E_FAIL;
            m_Bones.emplace_back(pBone);
        }

        fin.read(reinterpret_cast<char*>(&m_eModelType), sizeof(MODELTYPE));

        fin.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));

        m_Meshes.reserve(m_iNumMeshes);

            for (size_t i = 0; i < m_iNumMeshes; i++)
            {
                    CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, &fin);
                    if (nullptr == pMesh)
                        return E_FAIL;

                    m_Meshes.emplace_back(pMesh);
            }

        fin.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));
        for (size_t i = 0; i < m_iNumMaterials; i++)
        {
            MESH_MATERIAL pMt{};    
            fin.read(reinterpret_cast<char*>(&pMt.isTextured), sizeof(_bool));
            if(pMt.isTextured)
            {
                for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
                {
                    _uint check;
                    fin.read(reinterpret_cast<char*>(&check), sizeof(_uint));
                    if (1 == check)
                    {
                        pMt.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, &fin);
                        if (nullptr == pMt.MaterialTextures[j])
                            return E_FAIL;
                    }
                }
            }
            m_Materials.emplace_back(pMt);
        }



        if (TYPE_ANIM == m_eModelType)
        {
            fin.read(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));

            for (size_t i = 0; i < m_iNumAnimations; i++)
            {
                CAnimation* pAnimation = CAnimation::Create(&fin);
                if (nullptr == pAnimation)
                    return E_FAIL;

                m_Animations.emplace_back(pAnimation);
            }

        }
        int a = 1;
    }
    fin.close();

    return S_OK;
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
        aiString		TextureFilePath;
        TextureFilePath=pAIMaterial->GetName();

        MESH_MATERIAL		MeshMaterial{};

        MeshMaterial.isTextured = false;

        for (size_t j = aiTextureType_NONE; j < AI_TEXTURE_TYPE_MAX; j++)
        {
            aiString		strTextureFilePath;
            //pAIMaterial->GetTextureCount();
            if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTextureFilePath)))
                continue;

            MeshMaterial.isTextured = true;

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

HRESULT CModel::Ready_Animations()
{

    /* � �ִϸ��̼��� ������� ��, �� �ִϸ��̼ǿ��� ����ϰ� �ִ� ������ ������ ��ȯ ����(���)���� �ε��Ѵ�.  */
    m_iNumAnimations = m_pAIScene->mNumAnimations;

    for (size_t i = 0; i < m_iNumAnimations; i++)
    {
        CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.emplace_back(pAnimation);
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

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,  char* BinaryFilePath)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->Read_Binary(BinaryFilePath)))
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

    for (auto& pAnimation : m_Animations)
        Safe_Release(pAnimation);

    m_Animations.clear();

    for (auto& pBone : m_Bones)
        Safe_Release(pBone);

    m_Bones.clear();

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
