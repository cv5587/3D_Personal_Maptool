#pragma once

#include "Base.h"
#include <Layer.h>

/* ������ü���� �����Ѵ�. */
/* Ư�� ������ü�� �˻��Ͽ� �����ϰ�, ������ ��ü(�纻)�� ����(�����Ҵ�)���� �׷�(Layer)���� �����Ѵ�. */
/* �����ϰ� �ִ� �纻 ��ü���� �ݺ����� Tick�Լ����� ȣ�����ش�. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
public:
	typedef struct {
		_matrix PrePosition;
		wstring ClassTag;
		wstring ModelTag;
	}GAMEDATA_DESC;

private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex);

public:	
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	//�ͷ��� ��
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, CGameObject** pGameObject, void* pArg);
	HRESULT Delete_CloneObject(_uint iLevelIndex,  CGameObject* pGameObject);
	CGameObject* Find_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pGameObject);
	CGameObject* FindID_CloneObject(_uint iLevelIndex, const _int& ID);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
	vector< const _float4x4*>* Get_ObPos(_uint iLevelIndex, const wstring& strLayerTag);
	//���� ������Ʈ��
	class CGameObject* Clone_Object(const wstring& strPrototypeTag, void* pArg);

	//������ �Ľ�
public:
	HRESULT Save_Level(_uint iLevelIndex);

private:
	map<const wstring, class CGameObject*>				m_Prototypes;
	map<const wstring, class CLayer*>*					m_pLayers = { nullptr };
	_int											m_ObjectID = { 0 };
	_uint												m_iNumLevels = { 0 };

private:
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);


public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END