#ifndef  InputDev_h__
#define InputDev_h__

#include "Base.h"

#define KEYDOWN(name, key) (name[key] & 0x80) 



BEGIN(Engine)
class ENGINE_DLL CInput_Device :
    public CBase
{

private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;

public:
	_byte	Get_DIKeyState(_ubyte byKeyID) 
	{ return KEYDOWN(m_byKeyState,byKeyID); }
	_byte		Get_DIKeyState_Once(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
	{
		return KEYDOWN(m_tMouseState.rgbButtons,eMouse);
	}
	_byte	Get_DIMouseState_Once(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + eMouseState);
	}

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_byKeyState[256];		// Ű���忡 �ִ� ��� Ű���� �����ϱ� ���� ����
	DIMOUSESTATE			m_tMouseState;
	_bool key_down = false;
	_bool mouse_down = { false };
public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void	Free(void);

};

END
#endif//InputDev_h_