#pragma once
#include "Component.h"
BEGIN(Engine)
class ENGINE_DLL CModel final:
    public CComponent
{
public :
	enum MODELTYPE{TYPE_NONANIM,TYPE_ANIM,TYPE_END};
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

public:
	virtual HRESULT	Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT	Initialize(void* pArg) override;
	HRESULT Render(_uint iMeshIndex);

	HRESULT Bind_Material(class CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
	HRESULT Bind_BoneMatrices(class CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex);
	void Play_Animation(_float fTimeDelta);

private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;
	_float4x4					m_PreTransformMatrix;
	MODELTYPE					m_eModelType = { TYPE_END };
	_float4x4					m_MeshBoneMatrices[512];

private:
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>		m_Materials;

	vector<class CBone*>	m_Bones;

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END