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

private:
    _char       m_szName[MAX_PATH] = " ";
    _uint       m_iBoneIndex = { 0 };

    _uint				m_iNumKeyFrames = { 0 };
    vector<KEYFRAME>	m_KeyFrames;
public:
    static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
    virtual void Free() override;
};

END