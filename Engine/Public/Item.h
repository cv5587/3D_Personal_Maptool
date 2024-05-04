#pragma once
#include "GameObject.h"
BEGIN(Engine)
class ENGINE_DLL CItem :
	public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC {
		_uint iQuantity;
		_uint ItemType[2];
		wstring ItemName;
	}ITEM_DESC;

	enum ITEMTYPE {
		ITEM_STUFF,
		ITEM_MEDKITS,
		ITEM_KINDLING,
		ITEM_CLOTH,
		ITEM_FOOD,
		ITEM_EQUIPMENT,
		ITEM_END
	};

protected:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CGameObject& rhs);
	virtual ~CItem() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	wstring Get_ItemName() { return m_ItemName; }

	virtual void Make_Description(void* pArg) override;

	void* Get_Description();

	HRESULT Save_Data(ofstream* fout);

protected:
	vector<ITEMTYPE> m_ItemType;
	_uint			 m_Quantity = { 1 };
	wstring		m_ItemName = { TEXT("") };

public:
	virtual CGameObject* Clone(void* pArg) = 0;//������ Ŭ�󿡼� �����ϸ� ���ΰ� ���ؼ� ��������� �߻�
	virtual void Free() override;
};

END