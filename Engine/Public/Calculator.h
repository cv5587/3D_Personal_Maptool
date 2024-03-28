#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CCalculator :
    public CBase
{
	enum {FS_NEAR,FS_FAR, FS_LEFT, FS_RIGHT , FS_TOP,FS_BOT,FS_END};
private:
	CCalculator();
	virtual ~CCalculator() = default;

public:
	 HRESULT Initialize();
	_vector Picking_on_Terrain(HWND hWnd, _matrix TerrainWorldMatrixInverse, _matrix mViewMatrixInverse, _matrix mProjMatrixInverse,_float4* pVtxPos,_int* pTerrainUV, _float* pWinSize);
	 _bool Pick_Object(_matrix InverseView, _matrix InverseProj,  vector< const _float4x4*>* ObPos, _float radius);
public:
	 _bool Compare_Float4(_float4 f1, _float4 f2);	

private:
	 _float3 m_FrustumPoints[8] = {  };
	 //XMPlaneFromPoints();
public:
	static CCalculator* Create();
	virtual void Free() override;
};
END
