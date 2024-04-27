#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "Weapon.h"
#include "StateMachine.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CLandObject{ rhs }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CLandObject::LANDOBJ_DESC* pDesc = (CLandObject::LANDOBJ_DESC*)pArg;

	pDesc->fSpeedPerSec = 2.f;
	pDesc->fRotationPerSec = XMConvertToRadians(120.0f);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State_Matrix(XMLoadFloat4x4(&pDesc->vPrePosition));

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pStateMachine = CStateMachine::GetInstance();
	m_pStateMachine->Initialize();

	m_pStateMachine->Set_CurrentState(this, PLAYER_IDLE);

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	/* �÷��̾��� Transform�̶� �༮�� ������ �� �ٵ�� ������ �θ� ��������� ������ ������Ʈ�� �ɰŴ�. */

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Priority_Tick(fTimeDelta);

	m_pNavigationCom->Set_Cells(dynamic_cast<CNavigation*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Navigation")))->Get_Cells());

}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_eEquip != PLAYEREQUIP::EQUIP_STONE)
	{
		if (m_pGameInstance->Get_DIKeyState_Once(DIK_I))
		{
			Set_Equip(PLAYEREQUIP::EQUIP_STONE);
			Set_State(PLAYERSTATE::PLAYER_EQUIP);
		}
	}

	if (m_pGameInstance->Get_DIKeyState_Once(DIK_O))
	{
		Set_State(PLAYERSTATE::PLAYER_UNEQUIP);
	}

	if(m_pGameInstance->Get_DIKeyState_Once(DIK_E))
	{
		m_KeyInput = !m_KeyInput;
	}

	if(!m_KeyInput)
		m_pStateMachine->Update(this, fTimeDelta);

	//if (FAILED(__super::SetUp_OnTerrain(m_pTransformCom)))
	//	return;

	m_pNavigationCom->Set_OnNavigation(m_pTransformCom);

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer::Render()
{

#ifdef _DEBUG
	m_pColliderCom->Render();
	m_pNavigationCom->Render();
#endif

	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(0.3f, 0.7f, 0.3f);//aabb ��������
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC	NavigationDesc{};

	NavigationDesc.iCurrentCellIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
	/* �ٵ�ü�� �����ؿ´�. */
	CPartObject::PARTOBJ_DESC		BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	BodyDesc.fSpeedPerSec = 0.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.pState = &m_eState;
	BodyDesc.pEquip = &m_eEquip;
	BodyDesc.pAnimFinished = &m_bAnimFinished;

	CGameObject* pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Player"), &BodyDesc);
	if (nullptr == pBody)
		return E_FAIL;
	m_PartObjects.emplace_back(pBody);



	///* ���ⰴü�� �����ؿ´�. */
	//CWeapon::WEAPON_DESC			WeaponDesc{};
	//WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	////WeaponDesc.pState = &m_iState;
	//WeaponDesc.ProtoTypeTag = TEXT("Prototype_GameObject_Weapon");
	//WeaponDesc.ModelTag = TEXT("Prototype_Component_Model_Stone");
	//XMStoreFloat4x4(&WeaponDesc.vPrePosition, XMMatrixIdentity());	

	////���� ������ ������
	//CModel* pModelCom = dynamic_cast<CModel*>(pBody->Get_Component(TEXT("Com_Model")));
	//if (nullptr == pModelCom)
	//	return E_FAIL;
	////���� �� �پ� ���� �������� ������
	//WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("right_prop_point");
	//if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
	//	return E_FAIL;
	////�������� �־��༭ ����
	//CGameObject* pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	//if (nullptr == pWeapon)
	//	return E_FAIL;
	//m_PartObjects.emplace_back(pWeapon);

	return S_OK;
}

void CPlayer::Go_Straight(_float fTimeDelta)
{
	m_pTransformCom->Go_Straight(fTimeDelta,m_pNavigationCom);
}

void CPlayer::Go_Backward(_float fTimeDelta)
{
	m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);
}

void CPlayer::Go_Left(_float fTimeDelta)
{
	m_pTransformCom->Go_Left(fTimeDelta, m_pNavigationCom);
}

void CPlayer::Go_Right(_float fTimeDelta)
{
	m_pTransformCom->Go_Right(fTimeDelta, m_pNavigationCom);
}

void CPlayer::Go_LeftStraight(_float fTimeDelta)
{
	m_pTransformCom->Go_LeftStraight(fTimeDelta, m_pNavigationCom);
}

void CPlayer::Go_RightStraight(_float fTimeDelta)
{
	m_pTransformCom->Go_RightStraight(fTimeDelta, m_pNavigationCom);
}

void CPlayer::Go_LeftBackward(_float fTimeDelta)
{
	m_pTransformCom->Go_LeftBackward(fTimeDelta, m_pNavigationCom);
}

void CPlayer::Go_RightBackward(_float fTimeDelta)
{
	m_pTransformCom->Go_RightBackward(fTimeDelta, m_pNavigationCom);
}

void CPlayer::Set_DeltaValue(_float _fSpeedPerSec, _float _fRotationPerSec)
{
	m_pTransformCom->Set_DeltaValue(_fSpeedPerSec, _fRotationPerSec);
}

void CPlayer::Set_State(PLAYERSTATE eNewState)
{
	m_eState = eNewState;
	m_pStateMachine->Set_CurrentState(this, eNewState);
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);

	Safe_Release(m_pColliderCom);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);

	m_PartObjects.clear();

	Safe_Release(m_pStateMachine);

	CStateMachine::DestroyInstance();
}