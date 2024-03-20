#pragma once
#include "Base.h"
BEGIN(Client)
class CTerrainManager :
    public CBase
{
public:
    CTerrainManager();
    virtual ~CTerrainManager() = default;

public:
    HRESULT Initialize();
    HRESULT Clone_Terrain(void* pArg);

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CGameObject* m_pTerrain = { nullptr };
public:
   static CTerrainManager* Create();
    virtual void Free() ;
};
END
