
//샘플링 어케 할지?
sampler PointSampler = sampler_state
{
	filter = min_mag_mip_Point;
	AddressU = WRAP;
	AddressV = WRAP;
};


sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = WRAP;
	AddressV = WRAP;
};

//어떻게 그릴지
RasterizerState		RS_Default
{
	FillMode = SOLID;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

RasterizerState RS_NoneCull
{
    FillMode = SOLID;
    CullMode = None;
    FrontCounterClockwise = false;
};

RasterizerState		RS_Cull_CW
{
	FillMode = SOLID;
	CullMode = FRONT;
	FrontCounterClockwise = false;
};

//깊이값어케 할지

//DepthEnable 깊이 테스트 활성화 할지말지

//DepthWriteMask 깊이 버퍼에 대한 쓰기 작업을 제어
        //D3D11_DEPTH_WRITE_MASK_ZERO	= 0, 깊이 안쓸거야! 
        //D3D11_DEPTH_WRITE_MASK_ALL	= 1 깊이 쓸거야!
DepthStencilState	DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = LESS_EQUAL;
};

DepthStencilState	DSS_None_Test_None_Write
{
	DepthEnable = false;
	DepthWriteMask = zero;	
};


//vector		vSrcColor = vector(1.f, 0.f, 0.f, 0.5f);
//vector		vDestColor = vector(0.f, 1.f, 0.f, 1.f);

//vector		vResult = vSrcColor.rgb * vSrcColor.a + 
//	vDestColor * (1.f - vSrcColor.a);

//블렌드 할지말지 만약에 하면 어케할지
BlendState		BS_Default
{
	BlendEnable[0] = false;
};

BlendState		BS_AlphaBlend
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

BlendState		BS_Blend
{
	BlendEnable[0] = true;
	SrcBlend = One;
	DestBlend = One;
	BlendOp = Add;
};