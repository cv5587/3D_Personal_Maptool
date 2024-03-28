#pragma once

#include "Component.h"

/* 1. Vertex Buffer + Index Buffer */
/* 2. ������ �ε����� ����(����)�� �����ϴ� �۾��� �����ϳ�. */
/* 3. ���� �ٸ� ������ �𵨵����� �����Ѵ٤�(�ڽ�Ŭ������ etc. Rect, Cube, Model.) */

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();

public:
	HRESULT Bind_Buffers();

protected:
	ID3D11Buffer*				m_pVB = { nullptr };
	ID3D11Buffer*				m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC			m_BufferDesc = {};
	D3D11_BUFFER_DESC			m_VBufferDesc = {};
	D3D11_BUFFER_DESC			m_IBufferDesc = {};

	D3D11_SUBRESOURCE_DATA		m_InitialData = {};

	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology = {};
	DXGI_FORMAT					m_iIndexFormat = {};
	_uint						m_iNumVertexBuffers = { 0 };
	_uint						m_iVertexStride = { 0 };
	_uint						m_iNumVertices = { 0 };
	_uint						m_iIndexStride = { 0 };
	_uint						m_iNumIndices = { 0 };

protected:
	HRESULT Create_Buffer(ID3D11Buffer** ppVIBuffer);
	

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END