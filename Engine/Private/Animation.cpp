#include "..\Public\Animation.h"
#include "Channel.h"



CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_Duration{ rhs.m_Duration }
	, m_TickPerSecond{ rhs.m_TickPerSecond }
	, m_CurrentPosition{ rhs.m_CurrentPosition }
	, m_isFinished{ rhs.m_isFinished }
	, m_iNumChannels{ rhs.m_iNumChannels }
	, m_CurrentKeyFrameIndices{ rhs.m_CurrentKeyFrameIndices }
	, m_Channels{ rhs.m_Channels }
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_Duration = pAIAnimation->mDuration;
	m_TickPerSecond = pAIAnimation->mTicksPerSecond;

	/* �� �ִϸ��̼��� �����ϴ� �ʿ��� ���� ���� */
	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(ifstream* fin)
{
	fin->read(reinterpret_cast<char*>(m_szName), MAX_PATH);
	fin->read(reinterpret_cast<char*>(&m_Duration), sizeof(_double));
	fin->read(reinterpret_cast<char*>(&m_TickPerSecond), sizeof(_double));
	fin->read(reinterpret_cast<char*>(&m_CurrentPosition), sizeof(_double));
	fin->read(reinterpret_cast<char*>(&m_isFinished), sizeof(_bool));
	fin->read(reinterpret_cast<char*>(&m_iNumChannels), sizeof(_uint));

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	m_Channels.reserve(m_iNumChannels);
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(fin);
		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop)
{
	m_CurrentPosition += m_TickPerSecond * fTimeDelta;


	if (m_CurrentPosition >= m_Duration)
	{
		m_CurrentPosition = 0.0;

		if (false == isLoop)
			m_isFinished = true;
	}

	if (false == m_isFinished)
	{
		_uint		iChannelIndex = { 0 };

		for (auto& pChannel : m_Channels)
		{
			pChannel->Update_TransformationMatrix(m_CurrentPosition, Bones, &m_CurrentKeyFrameIndices[iChannelIndex++]);
		}
	}
}

_bool CAnimation::Shift_Animation_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones)
{

	//m_CurrentPosition = 0; // �ִ� ��ȯ�� �̹� ����Reset�Լ�����

	m_ShiftCurrentPosition+= (_double)fTimeDelta;

	if (m_ShiftCurrentPosition >= m_ShiftDuration)
	{
		m_ShiftCurrentPosition = 0.0;
		m_First_Shift = true;
		return true;
	}

	_uint		iChannelIndex = { 0 };

	for (auto& pChannel : m_Channels)
	{
		pChannel->Shift_Animation_TransformationMatrix(m_ShiftCurrentPosition, Bones, m_First_Shift);
	}

	m_First_Shift = false;

	return false;
}

void CAnimation::Reset()
{
	m_CurrentPosition = 0.0;
	m_isFinished = false;
	//�޸�
	vector<_uint>(m_CurrentKeyFrameIndices.size(), 0).swap(m_CurrentKeyFrameIndices);
}

void CAnimation::Shift_Reset()
{
	m_ShiftCurrentPosition = 0.0 ;
	m_First_Shift =  true ;
}

HRESULT CAnimation::Save_Animation(ofstream* fout)
{
	fout->write(reinterpret_cast<char*>(m_szName), MAX_PATH);
	fout->write(reinterpret_cast<char*>(&m_Duration), sizeof(_double));
	fout->write(reinterpret_cast<char*>(&m_TickPerSecond), sizeof(_double));
	fout->write(reinterpret_cast<char*>(&m_CurrentPosition), sizeof(_double));
	fout->write(reinterpret_cast<char*>(&m_isFinished), sizeof(_bool));
	fout->write(reinterpret_cast<char*>(&m_iNumChannels), sizeof(_uint));
	//�̰� �ҷ��ö�. ä�� ������ŭ m_CurrentKeyFrameIndices���͸� resize���ֱ�

	for (auto& pChannel : m_Channels)
		pChannel->Save_Channel(fout);

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

CAnimation* CAnimation::Create(ifstream* fin)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(fin)))
	{
		MSG_BOX("Failed To Read : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
	m_LastKeys.clear();
}
