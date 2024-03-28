#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);

private:
	_char			m_szName[MAX_PATH] = "";
	_double			m_Duration = { 0.0 };
	_double			m_TickPerSecond = { 0.0 };

	_uint					m_iNumChannels = { 0 };
	vector<class CChannel*>	m_Channels;

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END