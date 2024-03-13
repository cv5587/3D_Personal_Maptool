#pragma once

#include "Base.h"

/* ���� ��ü���� �׸��� ������ �����ؾ��Ѵ�. */
/* ��κ��� ��ü���� ���� �׽�Ʈ�� ���ؼ� �˾Ƽ� ��������� �ǰ� �ִ�. */
/* ���̹����� ����� ���� �˾Ƽ� ���̰������ȴ� ������ ū �����̱��ѵ�. */
/* �̷� ���ؼ� ����� ���ۿ��� �����ϳ�. */
/* ���ۿ� 1. ��ī�̹ڽ�. : ��ī�̹ڽ��� ������ �۰� �׸���.  */
/* �ϴ� : ������ �ٸ� ��ü�鿡�� �������Ѵ�(��������׸���) + �ϴ��� ���̴� �ٸ���ü��� �񱳿����� �Ͼ���� �ȵȴ�.(�ϴ��� ���̸� ���̹��ۿ� ������� �ʴ´�.) */
/* ���ۿ� 2. ���ĺ��� :   */
/* ���� : ���� �׸������ϴ� ���� ������ �׷����ִ� ���� ���´�. */
/* ���ۿ� 3. UI.  */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	void Draw();

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	list<class CGameObject*>			m_RenderGroup[RENDER_END];

private:
	void  Render_Priority();
	void  Render_NonBlend();
	void  Render_NonLight();
	void  Render_Blend();
	void  Render_UI();


public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END