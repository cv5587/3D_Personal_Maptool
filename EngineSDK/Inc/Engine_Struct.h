#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	/* ������ �ʱ�ȭ�ϱ����� �ʿ��� ������ �������� ������. */
	typedef struct
	{
		bool			isWindowed;
		HWND			hWnd;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
	}ENGINE_DESC;

	typedef struct
	{
		class CTexture* MaterialTextures[AI_TEXTURE_TYPE_MAX];
	}MESH_MATERIAL;

	typedef struct ENGINE_DLL 
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];

	}VTXPOSTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[3];

	}VTXNORTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;  //���ο����

		static const unsigned int		iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[4];

	}VTXMESH;

}
#endif // Engine_Struct_h__
