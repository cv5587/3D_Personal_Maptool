#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
    strcpy_s(m_szName, pAIChannel->mNodeName.data); 

    _uint iBoneIndex = { 0 };

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
		{
			if (true == pBone->Compare_Name(m_szName))
				return true;

			++iBoneIndex;

			return false;
		});

	m_iBoneIndex = iBoneIndex;


	/* 이 뼈의 전체 애니메이션 과정 중, 특정 시간대에 취해야할 상태들을 모두 다 읽어서 저장해놓는다.*/

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	KEYFRAME			KeyFrame{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		if (pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&KeyFrame.vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mScalingKeys[i].mTime;
		}
		if (pAIChannel->mNumRotationKeys > i)
		{
			KeyFrame.vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			KeyFrame.vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			KeyFrame.vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			KeyFrame.vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.Time = pAIChannel->mRotationKeys[i].mTime;
		}
		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&KeyFrame.vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mPositionKeys[i].mTime;
		}

		m_KeyFrames.emplace_back(KeyFrame);
	}

	return S_OK;
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX("Failed To Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	m_KeyFrames.clear();
}
