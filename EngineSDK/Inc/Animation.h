#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	_bool Get_Finished() const {
		return m_isFinished;

	}

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	HRESULT Initialize(ifstream* fin);
	void Update_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop);
	_bool Shift_Animation_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones);
	void Reset();
	void Shift_Reset();

	HRESULT Save_Animation(ofstream* fout);
private:
	_char			m_szName[MAX_PATH] = "";
	_double			m_Duration = { 0.0 };
	_double			m_TickPerSecond = { 0.0 };
	_double			m_CurrentPosition = { 0.0 };
	_bool			m_isFinished = { false };

	_uint					m_iNumChannels = { 0 };
	
	//�� ä�ε��� ���� ��ġ(currentposition) �� ���ʰ��� ä���� ������ŭ ������� ��Ƶ�
	vector<_uint>			m_CurrentKeyFrameIndices;
	//���� �Ʒ� �� ä���� ���� ���� ��ġ�� ���� Ű����
	vector<class CChannel*>	m_Channels;
	vector<KEYFRAME> m_LastKeys;
	
	_double		m_ShiftDuration = { 0.2 };
	_double		m_ShiftCurrentPosition = { 0.0 };
	_bool			m_First_Shift = { true };
public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	static CAnimation* Create(ifstream* fin);
	CAnimation* Clone();
	virtual void Free() override;
};

END