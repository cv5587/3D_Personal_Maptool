#pragma once
#include "Base.h"
BEGIN(Engine)
class CChannel final:
    public CBase
{
private:    
    CChannel();
    virtual ~CChannel() = default;
public:
    HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
    HRESULT Initialize(ifstream* fin);
    void Update_TransformationMatrix(_double CurrentPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex);
    void Shift_Animation_TransformationMatrix(_double ShiftCurrentPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex);

    HRESULT Save_Channel( ofstream* fout);
private:
    //ä�� �̸�
    _char       m_szName[MAX_PATH] = " ";
    //����޴� ���� �ε���
    _uint       m_iBoneIndex = { 0 };
    //Ű������ ����
    _uint				m_iNumKeyFrames = { 0 };
    //�� Ű�����Ӻ� ���� ��(scale,rotation(���ʹϾ�),position,time)
    vector<KEYFRAME>	m_KeyFrames;

    _vector			m_vPreScale, m_vPreRotation, m_vPreTranslation;
    _bool            m_bGetShiftBone = { false };
public:
    static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
    static CChannel* Create(ifstream* fin);
    virtual void Free() override;
};

END