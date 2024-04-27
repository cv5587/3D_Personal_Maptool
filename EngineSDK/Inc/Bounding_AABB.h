#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
	}BOUNDING_AABB_DESC;
private:
	CBounding_AABB();
	virtual ~CBounding_AABB() = default;

public:
	virtual void* Get_Desc() override {
		return m_pDesc;
	}

public:
	HRESULT Initialize(const void* pArg);
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::TYPE eTargetType, CBounding* pTargetBounding) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:
	BoundingBox* m_pOriginalDesc = { nullptr };//������ ������ ��ġ
	BoundingBox* m_pDesc = { nullptr };//���尡 ��������ġ 

private:
	_bool Intersect_AABB(const BoundingBox* pTargetBounding);

	_float3 Compute_Min(const BoundingBox* pTargetBounding);
	_float3 Compute_Max(const BoundingBox* pTargetBounding);

public:
	static CBounding_AABB* Create(const void* pArg);
	virtual void Free() override;
};

END