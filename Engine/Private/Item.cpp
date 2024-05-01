#include "Item.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice,pContext }
{
}

CItem::CItem(const CGameObject& rhs)
	:CGameObject{ rhs }
{
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);
	m_ItemName = pDesc->ItemName;
	m_Quantity = pDesc->iQuantity;

	m_ItemType.emplace_back((ITEMTYPE)pDesc->ItemType[0]);
	m_ItemType.emplace_back((ITEMTYPE)pDesc->ItemType[1]);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CItem::Priority_Tick(_float fTimeDelta)
{
}

void CItem::Tick(_float fTimeDelta)
{
}

void CItem::Late_Tick(_float fTimeDelta)
{
}

HRESULT CItem::Render()
{
	return S_OK;
}

void CItem::Make_Description(void* pArg)
{
	ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);

	pDesc->ItemType[0] = (_uint)m_ItemType[0];
	pDesc->ItemType[1] = (_uint)m_ItemType[1];

	pDesc->iQuantity = m_Quantity;

	pDesc->ItemName = m_ItemName;
}

void* CItem::Get_Description()
{
	ITEM_DESC Desc{};

	Make_Description(&Desc);

	__super::Make_Description(&Desc);

	return &Desc;
}

HRESULT CItem::Save_Data(ofstream* fout)
{
	_tchar ProtoTag[MAX_PATH] = TEXT("");
	_tchar ModelTag[MAX_PATH] = TEXT("");

	wsprintf(ProtoTag, m_ProtoTypeTag.c_str());
	wsprintf(ModelTag, m_ModelTag.c_str());

	fout->write((char*)ProtoTag, sizeof(_tchar) * MAX_PATH);
	fout->write((char*)ModelTag, sizeof(_tchar) * MAX_PATH);

	_float4x4 pPosition{};
	XMStoreFloat4x4(&pPosition, m_pTransformCom->Get_WorldMatrix());
	fout->write((char*)&pPosition, sizeof(_float4x4));

	_tchar ItemName[MAX_PATH] = TEXT("");

	wsprintf(ItemName, m_ItemName.c_str());

	fout->write((char*)ItemName, sizeof(_tchar) * MAX_PATH);

	fout->write((char*)&m_Quantity, sizeof(_uint));
	fout->write((char*)&m_ItemType[0], sizeof(_uint));
	fout->write((char*)&m_ItemType[1], sizeof(_uint));


	return S_OK;
}

void CItem::Free()
{
	__super::Free();
}
