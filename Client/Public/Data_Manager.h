#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CData_Manager :
    public CBase
{
private:
	CData_Manager(class CTerrainManager* pTerrainManager);
	virtual ~CData_Manager() = default;

	
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CTerrainManager* m_pTerrainMgr = { nullptr };

public:
	HRESULT Load_Data(_uint iLevelIndex);
public:
	HRESULT Initialize();
	static CData_Manager* Create(class CTerrainManager* pTerrainManager);

public:
	virtual void Free() override;
};

END