#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CGui final:    public CBase
{
private:
	 CGui();
	 virtual ~CGui();

public:
	HRESULT Initialize() ;
	static CGui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Render();

public:
	void Update_UI();
	virtual void Free() override;
};

END