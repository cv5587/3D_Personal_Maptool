#pragma once

#include "Base.h"

BEGIN(Engine)

/* 1. ID3D11Device, ID3D11DeviceContext��� ��ü�� �켱 �����Ѵ�. */
/* 2. IDXGISwapChain�� �����Ѵ�.(�����(ID3D11Texture2D)�� ���� �����Ѱž�.) */
/* 3. ����ۺ並 �����Ѵ�. */
/* 4. ���̹��� �ؽ��ĸ� �����ϰ� ���̹��� �並 �����Ѵ�. */

class CGraphic_Device final : public CBase
{	
public:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:

	/* �׷��� ����̽��� �ʱ�ȭ. */
	HRESULT Initialize(const ENGINE_DESC& EngineDesc, 
		_Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);

	/* ����۸� �����. */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);

	/* HitScreen�� �����. */
	HRESULT Clear_HitScreenBuffer_View();

	/* HitScreen�� �����. */
	HRESULT Clear_IDScreenBuffer_View();

	template<typename T>
	HRESULT Clear_Texture(TextureType eTextureType, T* data);

	/* ���̹��� + ���ٽǹ��۸� �����. */
	HRESULT Clear_DepthStencil_View();
	
	/* �ĸ� ���۸� ������۷� ��ü�Ѵ�.(����۸� ȭ�鿡 ���� �����ش�.) */
	HRESULT Present();

public:
	_float Compute_ProjZ(const POINT& ptWindowPos, ID3D11Texture2D* pHitScreenTexture);
	_int	Compute_ID(const POINT& ptWindowPos, ID3D11Texture2D* pIDScreenTexture);
private:	
	// IDirect3DDevice9* == LPDIRECT3DDEVICE9 == ID3D11Device + ID3D11DeviceContext 

	/* �޸� �Ҵ�. (��������, �ε�������, �ؽ��ķε�) �İ�ü�� ������ ���õ� ���� */
	ID3D11Device*				m_pDevice = { nullptr };
		
	/* ��ɽ���.(���ε��۾�, �������۸� SetStreamSource(), SetIndices(), SetTransform(), SetTexture() */
	/* �׸���. */
	ID3D11DeviceContext*		m_pDeviceContext = { nullptr };

	/* �ĸ���ۿ� ������۸� ��ü�ذ��鼭 ȭ�鿡 �����ִ� ���� */
	IDXGISwapChain*				m_pSwapChain = { nullptr };

	ID3D11Texture2D* m_pHitScreenTexture = { nullptr };

	ID3D11Texture2D* m_pIDScreenTexture = { nullptr };



	/* IDirect3DTexture9 */
	/* ID3D11Texture2D : �ؽ��ĸ� ǥ���ϴ� ������ü Ÿ���̴�.
	why? �뵵�� �´� ���������� ����ϱ� ���� �ؽ��İ�ü�� ����������.  */	

	/* ID3D11ShaderResourceView : ���̴��� ���޵� �� �ִ� �ؽ�ó Ÿ��. */	
	/* ID3D11RenderTargetView : ����Ÿ�ٿ����� ���� �� �ִ� �ؽ�ó Ÿ��. */
	/* ID3D11DepthStencilView : ���̽��ٽ� ���۷μ� ���� �� �ִ� Ÿ��.  */
	ID3D11RenderTargetView*		m_pBackBufferRTV = { nullptr };
	ID3D11RenderTargetView*		m_pHitScreenRTV = { nullptr };
	ID3D11RenderTargetView*		m_pIDScreenRTV = { nullptr };
	ID3D11DepthStencilView*		m_pDepthStencilView = { nullptr };
	D3D11_TEXTURE2D_DESC		m_HitTextureDesc{};
	D3D11_TEXTURE2D_DESC		m_IDTextureDesc{};
private:
	/* ����ü�ο��� �ʼ������� �ʿ��� �����ʹ� ����۰� �ʿ��Ͽ� ����۸� �����ϱ����� ������ �����ش�. */
	/* ����ü���� ������� == �����(�ؽ���)�� �����ȴ�. */
	HRESULT Ready_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_HitScreenRenderTargetView(_uint iWinCX, _uint iWinCY);
	HRESULT Ready_IDScreenRenderTargetView(_uint iWinCX, _uint iWinCY);
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	static CGraphic_Device* Create(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice,
		_Out_ ID3D11DeviceContext** ppDeviceContextOut);
	virtual void Free() override;
};

END

//���ø� ���Ǵ� �׻� inline���� �������.
template<typename T>
HRESULT CGraphic_Device::Clear_Texture(TextureType eTextureType, T* data)
{
	D3D11_TEXTURE2D_DESC* pTextureDesc = { nullptr };
	ID3D11Texture2D* pTexture = { nullptr };

	if (eTextureType == TextureType::HitPosTexture)
	{
		pTextureDesc = &m_HitTextureDesc;
		pTexture = m_pHitScreenTexture;
	}
	else if(eTextureType == TextureType::HitIDTexture)
	{
		pTextureDesc = &m_IDTextureDesc;
		pTexture = m_pIDScreenTexture;
	}


	switch ((*pTextureDesc).Usage)
	{
	case D3D11_USAGE_DEFAULT:
	{
		//������ �ʱ�ȭ ���� ( ���� ���� �κ�)
		D3D11_BOX box;
		box.left = 0;
		box.right = (*pTextureDesc).Width;
		box.top = 0;
		box.bottom = (*pTextureDesc).Height;
		box.front = 0;
		box.back = 1;
		//constexpr �޸� �Ҵ��� ���� �ʰ� �ٷ� ������� �������� �Լ�==������ Ÿ�ֿ̹� ������� �˼� �ֵ�.
		if (eTextureType == TextureType::HitPosTexture && constexpr(std::is_same<T, float>::value))
		{
			float* pData = new float[(*pTextureDesc).Width * (*pTextureDesc).Height];
			float val = *reinterpret_cast<float*>(data);
			for (_uint i = 0; i < (*pTextureDesc).Width * (*pTextureDesc).Height; i++)
			{
				pData[i] = val;
			}

			m_pDeviceContext->UpdateSubresource(pTexture, 0, &box, pData, (*pTextureDesc).Width * sizeof(float), 0);

			delete[] pData;
		}
		else if (eTextureType == TextureType::HitIDTexture && constexpr(std::is_same<T, int>::value))	
		{
			int* pData = new int[(*pTextureDesc).Width * (*pTextureDesc).Height];
			int val = *reinterpret_cast<int*>(data);
			for (_uint i = 0; i < (*pTextureDesc).Width * (*pTextureDesc).Height; i++)
			{
				pData[i] = val;
			}

			m_pDeviceContext->UpdateSubresource(pTexture, 0, &box, pData, (*pTextureDesc).Width * sizeof(int), 0);

			delete[] pData;
		}
		else
		{
			MSG_BOX("T Type Is Not Same As Texture Data Type");
			return E_FAIL;
		}
	}
	break;
	default:
		MSG_BOX("textureDesc Usage Is Not Default Usage");
		return E_FAIL;
	}

	return S_OK;
}
