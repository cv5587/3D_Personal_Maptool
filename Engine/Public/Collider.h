#pragma once

#include "Component.h"

/* �浹ü�� ������ ��´�. */
/* ����׿����� �����ϳ�. */
/* Ÿ �浹ü���� �浹�� üũ�Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

public:
	typedef struct
	{
		TYPE		eType = { TYPE_END };
	}COLLIDER_DESC;

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_fmatrix WorldMatrix);

public:
	_bool Intersect(CCollider* pTargetCollider);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	TYPE							m_eType = { TYPE_END };
	_bool							m_isColl = { false };
	class CBounding* m_pBounding = { nullptr };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pShader = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END