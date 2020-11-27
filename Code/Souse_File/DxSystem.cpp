#include "DxSystem.h"
using namespace DirectX;
//#include <crtdbg.h>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

ComPtr<ID3D11Device>		        DxSystem::Device;
ComPtr<IDXGISwapChain>		        DxSystem::SwapChain;

ComPtr<ID3D11DeviceContext>		    DxSystem::DeviceContext;
ComPtr<ID3D11RenderTargetView>      DxSystem::RenderTargetView;

ComPtr<ID3D11Texture2D>				DxSystem::DepthStencilTexture;
ComPtr<ID3D11DepthStencilView>		DxSystem::DepthStencilView;
ComPtr<ID3D11ShaderResourceView>	DxSystem::ShaderResourceView;
ComPtr<ID3D11DepthStencilState>		DxSystem::DepthStencilState[16];
ComPtr<ID3D11RasterizerState>		DxSystem::RasterizerState[RASTERIZE_TYPE];
ComPtr<ID3D11BlendState>			DxSystem::BlendState[BLEND_TYPE];

ComPtr<IDXGIDebug>                  DxSystem::DXGIDebug;
HWND								DxSystem::hwnd;
DXGI_SAMPLE_DESC					DxSystem::MSAA;
int DxSystem::ScreenWidth = 1920;
int DxSystem::ScreenHeight = 1080;
XMFLOAT4 DxSystem::Light_Direction = { 0.0f, 1.0f, 0.0f, 0 };
float DxSystem::elapsed_time = 0;

//****************************************************************
//
//	������
//
//****************************************************************
bool DxSystem::Initialize(HWND hWnd, int width, int height)
{
	hwnd = hWnd;
	CreateDevice();
	InitializeRenderTarget();
	CreateRasterizerState();
	CreateBlendState();
	return false;
}

//****************************************************************
//
//	�f�o�C�X����
//
//****************************************************************
HRESULT DxSystem::CreateDevice()
{
	HRESULT hr = S_OK;
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	// �f�o�b�O���C���[�̐ݒ�
	//creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//�f�o�C�X�̐���
	hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		creationFlags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		Device.GetAddressOf(),
		NULL,
		DeviceContext.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxW(hwnd, L"D3D11CreateDevice", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

#if defined(_DEBUG)
	/*
	//�f�o�b�O���[�h�J�n
	if (DXGIDebug == nullptr)
	{
		// �쐬
		typedef HRESULT(__stdcall* fPtr)(const IID&, void**);
		HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");
		fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");

		DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&DXGIDebug);

		// �o��
		DXGIDebug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
	}
	else
		DXGIDebug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
		*/
#endif
		///*
		//�g�p�\��MSAA���擾
	int max_count = 8;//D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
	if (max_count > 8)max_count = 8;
	for (int i = 0; i <= max_count; i++)
	{
		UINT Quality;
		if SUCCEEDED(Device->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &Quality))
		{
			if (0 < Quality)
			{
				MSAA.Count = i;
				MSAA.Quality = Quality - 1;
			}
		}
	}
	//*/

	//�C���^�[�t�F�[�X�擾
	IDXGIDevice1* hpDXGI = NULL;
	if (FAILED(Device.Get()->QueryInterface(__uuidof(IDXGIDevice1), (void**)&hpDXGI)))
	{
		MessageBoxW(hwnd, L"QueryInterface", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	//�A�_�v�^�[�擾
	IDXGIAdapter* hpAdapter = NULL;
	if (FAILED(hpDXGI->GetAdapter(&hpAdapter)))
	{
		MessageBoxW(hwnd, L"GetAdapter", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	//�t�@�N�g���[�擾
	IDXGIFactory* hpDXGIFactory = NULL;
	hpAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&hpDXGIFactory);
	if (hpDXGIFactory == NULL)
	{
		MessageBoxW(hwnd, L"GetParent", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	ScreenWidth = clientRect.right;
	ScreenHeight = clientRect.bottom;

	//�X���b�v�`�F�C���쐬
	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferDesc.Width = ScreenWidth;
	scd.BufferDesc.Height = ScreenHeight;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SampleDesc = MSAA;
	//scd.SampleDesc.Quality = 0;
	//scd.SampleDesc.Count = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if (FAILED(hpDXGIFactory->CreateSwapChain(Device.Get(), &scd, SwapChain.GetAddressOf())))
	{
		MessageBoxW(hwnd, L"CreateSwapChain", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	return S_OK;
}

void DxSystem::Release()
{
}

//****************************************************************
//
//	�����_�[�^�[�Q�b�g�֘A
//
//****************************************************************
//------------------------------------------------
//	������
//------------------------------------------------
bool DxSystem::InitializeRenderTarget()
{
	// �o�b�N�o�b�t�@�擾
	ID3D11Texture2D* BackBuffer = NULL;
	HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);

	if (FAILED(hr))
	{
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		return false;
	}

	// �����_�[�^�[�Q�b�g�r���[����
	hr = Device->CreateRenderTargetView(BackBuffer, NULL, RenderTargetView.GetAddressOf());
	BackBuffer->Release();
	BackBuffer = NULL;

	if (FAILED(hr))
	{
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		return false;
	}
	//  �[�x�X�e���V���o�b�t�@����
	CreateDepthStencil();

	// �r���[�|�[�g�̐ݒ�
	SetViewPort(ScreenWidth, ScreenHeight);

	return true;
}

//------------------------------------------------
//	�r���[�|�[�g�̐ݒ�
//------------------------------------------------
void DxSystem::SetViewPort(int width, int height, int Num)
{
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	DeviceContext->RSSetViewports(Num, &vp);
}

// �����_�[�^�[�Q�b�g�r���[�ݒ�
void DxSystem::SetDefaultView()
{
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
}

//------------------------------------------------
//	�[�x�X�e���V���o�b�t�@����
//------------------------------------------------
bool DxSystem::CreateDepthStencil()
{
	// �[�x�X�e���V���ݒ�
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = ScreenWidth;
	td.Height = ScreenHeight;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R32G8X24_TYPELESS;
	td.SampleDesc = MSAA;
	//td.SampleDesc.Count = 1;
	//td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	// �[�x�X�e���V���e�N�X�`������
	HRESULT hr = Device->CreateTexture2D(&td, NULL, &DepthStencilTexture);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �[�x�X�e���V���r���[�ݒ�
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvd.Texture2D.MipSlice = 0;

	// �[�x�X�e���V���r���[����
	hr = Device->CreateDepthStencilView(DepthStencilTexture.Get(), &dsvd, DepthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�f�v�X�X�e���V���X�e�[�g
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = FALSE;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::None)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::Less)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_GREATER;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::Greater)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::LEqual)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::GEqual)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_EQUAL;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::Equal)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::NotEqual)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::Always)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::None_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::Less_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_GREATER;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::Greater_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::LEqual_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::GEqual_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_EQUAL;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::Equal_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::NotEqual_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[static_cast<int>(DS_State::Always_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �V�F�[�_���\�[�X�r���[�ݒ�
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	// �V�F�[�_���\�[�X�r���[����
	hr = Device->CreateShaderResourceView(DepthStencilTexture.Get(), &srvd, ShaderResourceView.GetAddressOf());
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

//------------------------------------------------
//	���X�^���C�U�X�e�[�g�̐���
//------------------------------------------------
bool DxSystem::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rd;
	for (int state = 0; state < RASTERIZE_TYPE; ++state)
	{
		switch (state)
		{
			case static_cast<int>(RS_State::Cull_Back):
				ZeroMemory(&rd, sizeof(rd));
				rd.FillMode = D3D11_FILL_SOLID;
				rd.CullMode = D3D11_CULL_BACK;
				rd.FrontCounterClockwise = TRUE;
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = TRUE;
				rd.ScissorEnable = FALSE;
				rd.MultisampleEnable = TRUE;
				rd.AntialiasedLineEnable = TRUE;
				break;

			case static_cast<int>(RS_State::Cull_Front):
				ZeroMemory(&rd, sizeof(rd));
				rd.FillMode = D3D11_FILL_SOLID;
				rd.CullMode = D3D11_CULL_FRONT;
				rd.FrontCounterClockwise = TRUE;
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = TRUE;
				rd.ScissorEnable = FALSE;
				rd.MultisampleEnable = TRUE;
				rd.AntialiasedLineEnable = TRUE;
				break;

			case static_cast<int>(RS_State::Cull_None):
				ZeroMemory(&rd, sizeof(rd));
				rd.FillMode = D3D11_FILL_SOLID;
				rd.CullMode = D3D11_CULL_NONE;
				rd.FrontCounterClockwise = TRUE;
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = FALSE;
				rd.ScissorEnable = FALSE;
				rd.MultisampleEnable = TRUE;
				rd.AntialiasedLineEnable = TRUE;
				break;

			case static_cast<int>(RS_State::Standard):
				ZeroMemory(&rd, sizeof(rd));
				rd.FillMode = D3D11_FILL_SOLID;	// �h��Ԃ�
				rd.CullMode = D3D11_CULL_NONE;	// �J�����O
				rd.FrontCounterClockwise = TRUE;	// �O�p�`�̎��v��肪����
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = TRUE;	// �����Ɋ�Â��N���b�s���O��L����
				rd.ScissorEnable = FALSE;	// �V�U�[�����`�J�����O��L����
				rd.MultisampleEnable = TRUE;	// MSAA�Ŏl�ӌ`���A���t�@����ݒ�
				rd.AntialiasedLineEnable = TRUE;	// ���C��AA���L����
				break;

			case static_cast<int>(RS_State::Wire):
				ZeroMemory(&rd, sizeof(rd));
				rd.FillMode = D3D11_FILL_WIREFRAME;
				rd.CullMode = D3D11_CULL_BACK;
				rd.FrontCounterClockwise = TRUE;
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = TRUE;
				rd.ScissorEnable = FALSE;
				rd.MultisampleEnable = TRUE;
				rd.AntialiasedLineEnable = TRUE;
				break;
		}
		HRESULT hr = Device->CreateRasterizerState(&rd, RasterizerState[state].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	return true;
}

//�u�����h�X�e�[�g�̍쐬
bool DxSystem::CreateBlendState()
{
	D3D11_BLEND_DESC bd;

	for (int state = 0; state < BLEND_TYPE; state++)
	{
		switch (state)
		{
			case static_cast<int>(BS_State::Off):
				ZeroMemory(&bd, sizeof(bd));
				bd.IndependentBlendEnable = false;
				bd.AlphaToCoverageEnable = false;
				bd.RenderTarget[0].BlendEnable = false;
				bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				break;

			case static_cast<int>(BS_State::Alpha):
				ZeroMemory(&bd, sizeof(bd));
				bd.IndependentBlendEnable = false;
				bd.AlphaToCoverageEnable = false;
				bd.RenderTarget[0].BlendEnable = true;
				bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				break;

			case static_cast<int>(BS_State::Alpha_Test):
				ZeroMemory(&bd, sizeof(bd));
				bd.IndependentBlendEnable = false;
				bd.AlphaToCoverageEnable = true;
				bd.RenderTarget[0].BlendEnable = true;
				bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				//bd.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				break;

			case static_cast<int>(BS_State::Transparent):
				ZeroMemory(&bd, sizeof(bd));
				bd.IndependentBlendEnable = false;
				bd.AlphaToCoverageEnable = false;
				bd.RenderTarget[0].BlendEnable = true;
				bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				break;

			case static_cast<int>(BS_State::Add):
				ZeroMemory(&bd, sizeof(bd));
				bd.IndependentBlendEnable = false;
				bd.AlphaToCoverageEnable = false;
				bd.RenderTarget[0].BlendEnable = true;
				bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				break;

			case static_cast<int>(BS_State::Subtract):
				ZeroMemory(&bd, sizeof(bd));
				bd.IndependentBlendEnable = false;
				bd.AlphaToCoverageEnable = false;
				bd.RenderTarget[0].BlendEnable = true;
				bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;

				bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				break;

			case static_cast<int>(BS_State::Replace):
				ZeroMemory(&bd, sizeof(bd));
				bd.IndependentBlendEnable = false;
				bd.AlphaToCoverageEnable = false;
				bd.RenderTarget[0].BlendEnable = true;
				bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				break;
			case static_cast<int>(BS_State::Multiply):
				ZeroMemory(&bd, sizeof(bd));
				bd.IndependentBlendEnable = false;
				bd.AlphaToCoverageEnable = false;
				bd.RenderTarget[0].BlendEnable = true;
				bd.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
				bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
				bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				break;
		}
		//�u�����h�X�e�[�g�̍쐬
		HRESULT hr = Device->CreateBlendState(&bd, BlendState[state].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	return true;
}

//------------------------------------------------
//	�N���A
//------------------------------------------------
void DxSystem::Clear(DWORD color)
{
	// �����_�[�^�[�Q�b�g�r���[�ݒ�
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());

	float clearColor[4] = {1,0,0,0};
	/*
	for (int i = 3; i >= 0; i--)
	{
		clearColor[i] = ((color >> 8 * (3 - i)) & 0x00000000) / 255.0f;
	}
	*/
	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), clearColor);
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DeviceContext->OMSetDepthStencilState(DepthStencilState[static_cast<int>(DS_State::LEqual)].Get(), 1);
}

//------------------------------------------------
//	�t���b�v
//------------------------------------------------
void DxSystem::Flip(int n)
{
	// �t���b�v����
	SwapChain->Present(n, 0);
}