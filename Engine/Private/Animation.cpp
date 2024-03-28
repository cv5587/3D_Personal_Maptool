#include "..\Public\Animation.h"
#include "Channel.h"



CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_Duration = pAIAnimation->mDuration;
	m_TickPerSecond = pAIAnimation->mTicksPerSecond;

	/* 이 애니메이션을 구동하는 필요한 뼈의 갯수 */
	m_iNumChannels = pAIAnimation->mNumChannels;

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX("Failed To Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
