#include "..\Public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
{
}

CComponent* CObject_Manager::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(strComponentTag, iIndex);
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers = new map<const wstring, class CLayer*>[iNumLevels];

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{

	/* ���� �����ؾ��� ������� �˻��ϳ�. */
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);



	CGameObject* pCloneObject = pPrototype->Clone(pArg);
	if (nullptr == pCloneObject)
		return E_FAIL;

	pCloneObject->Set_ID(m_ObjectID++);
	/* ���� �߰��ҷ����ϴ� ���̾ ������?!! */
	/* ���̾ �����ؼ� �߰��Ϲȵǰδ�. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pCloneObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	/* ���� �߰��ҷ����ϤѤ��� ���̾ �־���!!  */
	else
		pLayer->Add_GameObject(pCloneObject);

	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, CGameObject** pGameObject, void* pArg)
{
	/* ���� �����ؾ��� ������� �˻��ϳ�. */
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);



	CGameObject* pCloneObject = pPrototype->Clone(pArg);
	if (nullptr == pCloneObject)
		return E_FAIL;

	pCloneObject->Set_ID(m_ObjectID++);
	/* ���� �߰��ҷ����ϴ� ���̾ ������?!! */
	/* ���̾ �����ؼ� �߰��Ϲȵǰδ�. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pCloneObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	/* ���� �߰��ҷ����ϤѤ��� ���̾ �־���!!  */
	else
		pLayer->Add_GameObject(pCloneObject);


	//�ּҰ����� ���� ���� ���� ��ü
	//���� �����ͷ� �ͷ��ΰ�ü�� �������� nullptr �� �ƴϱ� ������  
	// �ͷ��ο� ���ؼ��� delete��Ų��.
	if (nullptr != *pGameObject)
	{
		pLayer->Delete_GameObject(*pGameObject);
		*pGameObject = nullptr;
	}
	*pGameObject = pCloneObject;
	Safe_AddRef(pCloneObject);

	return S_OK;
}



HRESULT CObject_Manager::Delete_CloneObject(_uint iLevelIndex,  CGameObject* pGameObject)
{
	for (auto& pLayer : m_pLayers[iLevelIndex])
	{
		pLayer.second->Delete_GameObject(pGameObject);
	}

	return S_OK;
}

CGameObject* CObject_Manager::Find_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pGameObject)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/*�������̾ ������ */
	if (nullptr == pLayer)
		return nullptr;
	else
		return pLayer->Find_GameObject(pGameObject);
}

CGameObject* CObject_Manager::FindID_CloneObject(_uint iLevelIndex, const _int& ID)
{
	for(auto& pLayer : m_pLayers[iLevelIndex])
	{
		CGameObject* pGameObject= pLayer.second->Find_GameObject(ID);
		if (nullptr != pGameObject)
			return pGameObject;
	}
	return nullptr;
}

void CObject_Manager::Priority_Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Priority_Tick(fTimeDelta);
	}
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Tick(fTimeDelta);
	}
}

void CObject_Manager::Late_Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Late_Tick(fTimeDelta);
		}
	}	
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pLayers[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}
	m_pLayers[iLevelIndex].clear();
}


vector< const _float4x4*>* CObject_Manager::Get_ObPos(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second->Get_ObPos();
}

CGameObject* CObject_Manager::Clone_Object(const wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pCloneObject = pPrototype->Clone(pArg);
	if (nullptr == pCloneObject)
		return nullptr;
	pCloneObject->Set_ID(m_ObjectID++);

	return pCloneObject;
}

HRESULT CObject_Manager::Save_Level(_uint iLevelIndex)
{
	char FileRoute[MAX_PATH] = "../Bin/bin/Save_Data/";
	char FilePath[MAX_PATH] = "";
	char FileDat[MAX_PATH] = ".dat";

	switch (iLevelIndex)
	{
		//Ŭ�� ���� �����۾�����ߵ�
	case 0 :
		strcpy_s(FilePath, "LEVEL_STATIC_GAMEDATA");
		break;
	case 1:
		strcpy_s(FilePath, "LEVEL_LOADING_GAMEDATA");
		break;
	case 2:
		strcpy_s(FilePath, "LEVEL_LOGO_GAMEDATA");
		break;
	case 3:
		strcpy_s(FilePath, "LEVEL_GAMEPLAY_GAMEDATA");
		break;
	default:
		return E_FAIL;
	}

	strcat_s(FileRoute, FilePath);
	strcat_s(FileRoute, FileDat);

	_tchar Layer[MAX_PATH] = TEXT("");



	ofstream fout;
	fout.open(FileRoute, ios::out | ios::binary);

	if (!fout.fail())
	{
		fout.write((char*)&iLevelIndex,  sizeof(_uint));

		//������ ���̾� ������ ������
		_uint Layersize = m_pLayers[iLevelIndex].size();

		fout.write((char*)&Layersize, sizeof(_uint));

		for (auto& pLayer : m_pLayers[iLevelIndex])
		{
			ZeroMemory(Layer, sizeof(_tchar) * MAX_PATH);
			wsprintf(Layer, pLayer.first.c_str());
			wstring wLayer(Layer);

			//ī�޶�� �÷��̾ ���� ���� ����
			if (TEXT("Layer_Camera") != wLayer)
			{
				fout.write((char*)Layer, sizeof(_tchar) * MAX_PATH);
				if (FAILED(pLayer.second->Save_Data(&fout)))	
					return E_FAIL;
			}
		}
	}

	fout.close();

	return S_OK;
}

CGameObject * CObject_Manager::Find_Prototype(const wstring & strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;	
}


CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring & strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CObject_Manager * CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager*		pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed To Created : CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
}
