#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CCalculator :
    public CComponent
{
private:
	CCalculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCalculator(const CCalculator& rhs);
	virtual ~CCalculator() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	 _float4 Picking_on_Terrain(HWND hwnd, _matrix _ViewMatrixInverse, _matrix ProjMatrixInverse);
	 class CGameObject* Pick_Object();
	
public:
	//_float4 Picking_on_Terrain(_vector _vposition)
public:
	static CCalculator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
END
