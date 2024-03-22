#include "Calculator.h"

CCalculator::CCalculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CComponent{pDevice,pContext}
{
}

CCalculator::CCalculator(const CCalculator& rhs)
    :CComponent{rhs}
{
}

HRESULT CCalculator::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCalculator::Initialize(void* pArg)
{
    return S_OK;
}

_float4 CCalculator::Picking_on_Terrain(HWND hwnd,_matrix _ViewMatrixInverse,_matrix ProjMatrixInverse)
{


	return _float4();
}

CGameObject* CCalculator::Pick_Object()
{
	return nullptr;
}

CCalculator* CCalculator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCalculator* pInstance = new CCalculator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCalculator::Clone(void* pArg)
{
	CCalculator* pInstance = new CCalculator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCalculator::Free()
{
	__super::Free();
}
