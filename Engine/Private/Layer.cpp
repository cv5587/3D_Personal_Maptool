#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{

}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}


HRESULT CLayer::Delete_GameObject( CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return S_OK;

	list<CGameObject*>::iterator iter = m_GameObjects.begin();
	//auto& iter = m_GameObjects.begin();
	for (iter ; iter != m_GameObjects.end();)
	{
		/*������ ������ �ִ�*/
		if (pGameObject == *iter)
		{
			Safe_Release(pGameObject);
			Safe_Release(*iter);
			iter = m_GameObjects.erase(iter);
			break;
		}
		else
			iter++;
	}
	return S_OK;
}

CGameObject* CLayer::Find_GameObject(CGameObject* pGameObject)
{

	if (nullptr == pGameObject)
		return nullptr;

	list<CGameObject*>::iterator iter = m_GameObjects.begin();
	for (iter; iter == m_GameObjects.end(); iter++)
	{
		/*���� ��ȯ*/
		if (pGameObject == *iter)
			return *iter;
	}
	/*���� ������ ���п�*/
	return nullptr;

}

void CLayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Priority_Tick(fTimeDelta);
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Tick(fTimeDelta);
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Late_Tick(fTimeDelta);
}

CLayer * CLayer::Create()
{
	CLayer*		pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}