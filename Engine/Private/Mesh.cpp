#include "Mesh.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& rhs)
    : CVIBuffer{ rhs } 
{
}

HRESULT CMesh::Initialize(CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones)
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;

	m_iIndexStride = 4;
	m_iNumIndices = pAIMesh->mNumFaces * 3;

#pragma region VERTEX_BUFFER 

	HRESULT hr = eModelType == CModel::TYPE_NONANIM ? Ready_Vertices_For_NonAnimMesh(pAIMesh, PreTransformMatrix) : Ready_Vertices_For_AnimMesh(pAIMesh, Bones);
		
		if (FAILED(hr))
			return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER 
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

void CMesh::Fill_Matrices(vector<class CBone*>& Bones, _float4x4* pMeshBoneMatrices)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&pMeshBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix()));
	}
}


HRESULT CMesh::Ready_Vertices_For_NonAnimMesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix)));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_AnimMesh(const aiMesh* pAIMesh, const vector<class CBone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}
	/* 이 메시에 영향을 주는 뼈의 갯수를 가져온다. */
	m_iNumBones = pAIMesh->mNumBones;


	/* 뼈들를 순회하면서 각각의 뼈가 어떤 정점들에게 영향을 주는가를 캐치한다. */
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		/* pAIBone = i번째 뼈다 */
		aiBone* pAIBone = pAIMesh->mBones[i];

		/*pAIBone->mOffsetMatrix*/
		_float4x4		OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.emplace_back(OffsetMatrix);

		_uint		iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(pAIBone->mName.data))
					return true;

				++iBoneIndex;

				return false;
			});

		m_BoneIndices.emplace_back(iBoneIndex);

		/* pAIBone->mNumWeights : i번째 뼈가 몇개의 정점들에게 영향을 주는가? */
		/* j루프 : 이 뼈가 영향을 주는 정점들을 순회한다. */
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight		VertexWeight = pAIBone->mWeights[j];

			/* VertexWeight.mVertexId : 이 뼈가 영향을 주는 정점들 중, j번째 정점의 인덱스 */
			/* pVertices[VertexWeight.mVertexId].vBlendIndices: 이 정점에게 영햐응ㄹ 주는 뼈의 인덱스를 최대 네개 저장한다. */
			if (0.0f == pVertices[VertexWeight.mVertexId].vBlendWeights.x)
			{
				pVertices[VertexWeight.mVertexId].vBlendIndices.x = i;
				pVertices[VertexWeight.mVertexId].vBlendWeights.x = VertexWeight.mWeight;
			}

			else if (0.0f == pVertices[VertexWeight.mVertexId].vBlendWeights.y)
			{
				pVertices[VertexWeight.mVertexId].vBlendIndices.y = i;
				pVertices[VertexWeight.mVertexId].vBlendWeights.y = VertexWeight.mWeight;
			}

			else if (0.0f == pVertices[VertexWeight.mVertexId].vBlendWeights.z)
			{
				pVertices[VertexWeight.mVertexId].vBlendIndices.z = i;
				pVertices[VertexWeight.mVertexId].vBlendWeights.z = VertexWeight.mWeight;
			}

			else
			{
				pVertices[VertexWeight.mVertexId].vBlendIndices.w = i;
				pVertices[VertexWeight.mVertexId].vBlendWeights.w = VertexWeight.mWeight;
			}
		}
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODELTYPE eModelType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix, const vector<class CBone*>& Bones)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eModelType, pAIMesh, PreTransformMatrix, Bones)))
	{
		MSG_BOX("Failed To Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
    return nullptr;
}

void CMesh::Free()
{
    __super::Free();
}
