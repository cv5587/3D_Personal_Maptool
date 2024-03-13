#pragma once

#include "Engine_Defines.h"

/* ��� Ŭ�������� �θ� �Ǵ� Ŭ������. == SuperClass */
/* ���۷��� ī��Ʈ�� �����Ѵ�. AddRef, Release or Delete */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	_uint AddRef();
	_uint Release();

private:
	_uint		m_iRefCnt = { 0 };

public:
	virtual void Free() = 0;
};

END