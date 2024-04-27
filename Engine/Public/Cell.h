#pragma once

#include "Base.h"

/* 1. �׺���̼��� �����ϴ� �ϳ��� �ﰡ��. */
/* 2. �ﰢ���� �����ϴ� ������ ������ �����Ѵ�. */
/* 3. �� ���� �����Ͽ� ��ü�� �����ġ�� �ȿ� �ִ���? ������? ����.  */
BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	HRESULT Initialize_Data(const _float3* pPoints, _int iIndex, _int* Neighbor);
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_bool isIn(_fvector vPoint, _int* pNeighborIndex);
	void SetUp_Neighbor(LINE eNeighbor, CCell* pCell) {
		m_iNeighborIndices[eNeighbor] = pCell->m_iIndex;
	}
	_bool Check_Points(_vector vPoint, _float3* StorePoint, _float SnapReach);
	void Save_Data(ofstream* pfout);
	void Sorting(_int DeleteCellIndex);
	void NeighborSort(_int DeleteCellIndex);
#ifdef _DEBUG
public:
	HRESULT Render();

#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	_float3				m_vPoints[POINT_END] = {};
	_int					m_iIndex = { 0 };
	_int					m_iNeighborIndices[LINE_END] = { -1, -1, -1 };
public:
	_float Set_OnCell(_float3 vLocalPos);
#ifdef _DEBUG

private:
	class CVIBuffer_Cell*		m_pVIBuffer = { nullptr };

#endif
	
public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex,_int* Neighbor);
	virtual void Free() override;
};

END