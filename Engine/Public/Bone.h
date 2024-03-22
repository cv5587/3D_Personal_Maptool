#pragma once

#include "Base.h"

/* assimp������ ���� ǥ���ϱ����� Ÿ������ ������ Ÿ���� �������ش�. */
/* aiNode, aiBone, aiNodeAnim */

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _float4x4* Get_CombinedTransformationMatrix() const {
		return &m_CombinedTransformationMatrix;
	}

public:
	_bool Compare_Name(const _char* pName) {
		return !strcmp(m_szName, pName);
	}

public:
	HRESULT Initialize(const aiNode* pBoneNode, _int iParentIndex);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones);

private:
	_char				m_szName[MAX_PATH] = "";
	_int				m_iParentBoneIndex = { 0 };
	_float4x4			m_TransformationMatrix;
	_float4x4			m_CombinedTransformationMatrix;


public:
	static CBone* Create(const aiNode* pBoneNode, _int iParentIndex);
	virtual void Free() override;
};

END