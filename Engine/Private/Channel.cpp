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


	/* �� ���� ��ü �ִϸ��̼� ���� ��, Ư�� �ð��뿡 ���ؾ��� ���µ��� ��� �� �о �����س��´�.*/

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

HRESULT CChannel::Initialize(ifstream* fin)
{
	fin->read(reinterpret_cast<char*>(m_szName), MAX_PATH);
	fin->read(reinterpret_cast<char*>(&m_iBoneIndex), sizeof(_uint));
	fin->read(reinterpret_cast<char*>(&m_iNumKeyFrames), sizeof(_uint));

	m_KeyFrames.reserve(m_iNumKeyFrames);
	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME  KeyFrame{};
		fin->read(reinterpret_cast<char*>(&KeyFrame), sizeof(KEYFRAME));
		m_KeyFrames.emplace_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Update_TransformationMatrix(_double CurrentPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex)
{

	if (0.0 == CurrentPosition)
		*pCurrentKeyFrameIndex = 0;


	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	_vector			vScale, vRotation, vTranslation;

	/* ���� �����ġ�� ������ Ű�������� ��ġ�� �Ѿ�ԵǸ� ������ ������ Ű�������� ���¸� ���Ѵ�.  */
	if (CurrentPosition >= LastKeyFrame.Time)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
	}
	/* Ư�� Ű�����ӵ� ���̿� �����Ŵ�!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	else
	{
		while (CurrentPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].Time)
			++*pCurrentKeyFrameIndex;

		/* ���� ��ġ���� ���ʿ� �ִ� Ű�������� ��ġ�� ����. / �� ������ Ű�������� ��ġ - �� ���� Ű�������� ��ġ. */
		_float		fRatio = (CurrentPosition - m_KeyFrames[*pCurrentKeyFrameIndex].Time) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].Time - m_KeyFrames[*pCurrentKeyFrameIndex].Time);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation), fRatio);
		vTranslation = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition), fRatio);
		vTranslation = XMVectorSetW(vTranslation, 1.f);
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Shift_Animation_TransformationMatrix(_double ShiftCurrentPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex)
{

	if (!m_bGetShiftBone)
	{
		//�������� ������ ���� ��� ������ �����´�.
		_matrix m_ShiftBoneMatrix = XMLoadFloat4x4(Bones[m_iBoneIndex]->Get_TransformationMatrix());
		XMMatrixDecompose(&m_vPreScale, &m_vPreRotation, &m_vPreTranslation, m_ShiftBoneMatrix);
		m_bGetShiftBone = true;
	}
	_vector			vScale, vRotation, vTranslation;
	/* ���� �����ġ�� ������ Ű�������� ��ġ�� �Ѿ�ԵǸ� ������ ������ Ű�������� ���¸� ���Ѵ�.  */
	if (ShiftCurrentPosition >= 0.2)
	{
		vScale			= m_vPreScale;
		vRotation		= m_vPreRotation;
		vTranslation = m_vPreTranslation;
		m_bGetShiftBone = false;
	}
	/* Ư�� Ű�����ӵ� ���̿� �����Ŵ�!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	else
	{
		/* ���� ��ġ���� ���ʿ� �ִ� Ű�������� ��ġ�� ����. / �� ������ Ű�������� ��ġ - �� ���� Ű�������� ��ġ. */
		_float		fRatio = ShiftCurrentPosition  / 0.2 ;

		vScale = XMVectorLerp(m_vPreScale, XMLoadFloat3(&m_KeyFrames[0].vScale), fRatio);
		vRotation = XMQuaternionSlerp(m_vPreRotation, XMLoadFloat4(&m_KeyFrames[0].vRotation), fRatio);
		vTranslation = XMVectorLerp(m_vPreTranslation, XMLoadFloat3(&m_KeyFrames[0].vPosition), fRatio);
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

HRESULT CChannel::Save_Channel( ofstream* fout)
{
	fout->write(reinterpret_cast<char*>(m_szName), MAX_PATH);
	fout->write(reinterpret_cast<char*>(&m_iBoneIndex), sizeof(_uint));
	fout->write(reinterpret_cast<char*>(&m_iNumKeyFrames), sizeof(_uint));

	for (auto& KeyFrame:m_KeyFrames)
		fout->write(reinterpret_cast<char*>(&KeyFrame), sizeof(KEYFRAME));
		

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

CChannel* CChannel::Create(ifstream* fin)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(fin)))
	{
		MSG_BOX("Failed To Load : CChannel");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CChannel::Free()
{
	m_KeyFrames.clear();
}
