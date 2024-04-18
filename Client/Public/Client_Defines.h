#pragma once

#include <process.h>

namespace Client
{
#define SPEED_EX_WALK 1.f
#define SPEED_WALK 2.f

#define SPEED_EX_JOG 2.f
#define SPEED_JOG 3.f

#define SPEED_EX_RUN 3.f
#define SPEED_RUN 5.f

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
	//�ִϸ��̼� �б����
		enum PLAYERSTATE {
		PLAYER_IDLE,//�⺻
		PLAYER_IDLE_EXHAUSTED,//��ħ �⺻
		PLAYER_JOG,
		PLAYER_RUN,//�ٱ�
		PLAYER_EQUIP,//����
		PLAYER_UNEQUIP,//Ż��
		PLAYER_WALK_S,//�ȱ� ����
		PLAYER_WALK_E,//�ȱ�����
		PLAYER_WALK_FB,//�յ� �ȱ�
		PLAYER_SLOW_WALK_FB,//�յ� õõ�� �ȱ�
		PLAYER_WALK_L,//�·� �ȱ�
		PLAYER_WALK_R,//��� �ȱ�
		PLAYER_WALK_LE,//�·� �ȱ� ����
		PLAYER_WALK_RE,//��� �ȱ� ����
		PLAYER_AIM_S,//���ӽ���
		PLAYER_AIM,//������
		PLAYER_AIM_E,//��������
		PLAYER_THROW,//������
		PLAYER_ATTACK,//����(STIM(��,������,���������) ���� FPH_Gear_Stim_Inject)
		PLAYER_END
	};
	
	enum PLAYEREQUIP {
		EQUIP_NONE,
		EQUIP_STONE,
		EQUIP_PIPE,
		EQUIP_END
	};

	enum PLAYERCONDITION {
		CON_NORMAL,
		CON_EXHAUSTED,
		CON_END
	};

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
