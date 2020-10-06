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
ComPtr<ID3D11DepthStencilState>		DxSystem::DepthStencilState[3];
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
//	初期化
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
//	デバイス生成
//
//****************************************************************
HRESULT DxSystem::CreateDevice()
{
	HRESULT hr = S_OK;
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	// デバッグレイヤーの設定
	//creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//デバイスの生成
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
	//デバッグモード開始
	if (DXGIDebug == nullptr)
	{
		// 作成
		typedef HRESULT(__stdcall* fPtr)(const IID&, void**);
		HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");
		fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");

		DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&DXGIDebug);

		// 出力
		DXGIDebug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
	}
	else
		DXGIDebug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
		*/
#endif
		///*
		//使用可能なMSAAを取得
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

	//インターフェース取得
	IDXGIDevice1* hpDXGI = NULL;
	if (FAILED(Device.Get()->QueryInterface(__uuidof(IDXGIDevice1), (void**)&hpDXGI)))
	{
		MessageBoxW(hwnd, L"QueryInterface", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	//アダプター取得
	IDXGIAdapter* hpAdapter = NULL;
	if (FAILED(hpDXGI->GetAdapter(&hpAdapter)))
	{
		MessageBoxW(hwnd, L"GetAdapter", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	//ファクトリー取得
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

	//スワップチェイン作成
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
//	レンダーターゲット関連
//
//****************************************************************
//------------------------------------------------
//	初期化
//------------------------------------------------
bool DxSystem::InitializeRenderTarget()
{
	// バックバッファ取得
	ID3D11Texture2D* BackBuffer = NULL;
	HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);

	if (FAILED(hr))
	{
		//assert(FAILED(hr));
		return false;
	}

	// レンダーターゲットビュー生成
	hr = Device->CreateRenderTargetView(BackBuffer, NULL, RenderTargetView.GetAddressOf());
	BackBuffer->Release();
	BackBuffer = NULL;

	if (FAILED(hr))
	{
		//assert(FAILED(hr));
		return false;
	}
	//  深度ステンシルバッファ生成
	CreateDepthStencil();

	// ビューポートの設定
	SetViewPort(ScreenWidth, ScreenHeight);

	return true;
}

//------------------------------------------------
//	ビューポートの設定
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

// レンダーターゲットビュー設定
void DxSystem::SetDefaultView()
{
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
}

//------------------------------------------------
//	深度ステンシルバッファ生成
//------------------------------------------------
bool DxSystem::CreateDepthStencil()
{
	// 深度ステンシル設定
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = ScreenWidth;
	td.Height = ScreenHeight;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R24G8_TYPELESS;
	td.SampleDesc = MSAA;
	//td.SampleDesc.Count = 1;
	//td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	// 深度ステンシルテクスチャ生成
	HRESULT hr = Device->CreateTexture2D(&td, NULL, &DepthStencilTexture);
	//assert(FAILED(hr));

	if (FAILED(hr))
	{
		return false;
	}

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvd.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー生成
	hr = Device->CreateDepthStencilView(DepthStencilTexture.Get(), &dsvd, DepthStencilView.GetAddressOf());
	//assert(FAILED(hr));

	if (FAILED(hr))
	{
		return false;
	}

	//デプスステンシルステート
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = FALSE;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[DS_FALSE].GetAddressOf());
	//assert(FAILED(hr));
	if (FAILED(hr))
	{
		return false;
	}

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[DS_TRUE].GetAddressOf());
	//assert(FAILED(hr));
	if (FAILED(hr))
	{
		return false;
	}

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = Device->CreateDepthStencilState(&depth_stencil_desc, DepthStencilState[DS_SKY].GetAddressOf());
	//assert(FAILED(hr));
	if (FAILED(hr))
	{
		return false;
	}
	
	// シェーダリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	// シェーダリソースビュー生成
	hr = Device->CreateShaderResourceView(DepthStencilTexture.Get(), &srvd, ShaderResourceView.GetAddressOf());
	//assert(FAILED(hr));

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

//------------------------------------------------
//	ラスタライザステートの生成
//------------------------------------------------
bool DxSystem::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rd;
	for (int state = 0; state < RASTERIZE_TYPE; state++)
	{
		switch (state)
		{
			case RS_STANDARD:
				ZeroMemory(&rd, sizeof(rd));
				rd.FillMode = D3D11_FILL_SOLID;	// 塗りつぶし
				rd.CullMode = D3D11_CULL_BACK;	// カリング
				rd.FrontCounterClockwise = TRUE;	// 三角形の時計回りが正面
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = FALSE;	// 距離に基づくクリッピングを有効か
				rd.ScissorEnable = FALSE;	// シザー長方形カリングを有効か
				rd.MultisampleEnable = TRUE;	// MSAAで四辺形かアルファ線を設定
				rd.AntialiasedLineEnable = TRUE;	// ラインAAが有効か

				break;

			case RS_CULL_BACK:
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

			case RS_WIRE:
				ZeroMemory(&rd, sizeof(rd));
				rd.FillMode = D3D11_FILL_WIREFRAME;
				rd.CullMode = D3D11_CULL_BACK;
				rd.FrontCounterClockwise = FALSE;
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = TRUE;
				rd.ScissorEnable = FALSE;
				rd.MultisampleEnable = TRUE;
				rd.AntialiasedLineEnable = TRUE;
				break;

			case RS_CULL_FRONT:
				ZeroMemory(&rd, sizeof(rd));
				rd.FillMode = D3D11_FILL_SOLID;
				rd.CullMode = D3D11_CULL_FRONT;
				rd.FrontCounterClockwise = FALSE;
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = TRUE;
				rd.ScissorEnable = FALSE;
				rd.MultisampleEnable = TRUE;
				rd.AntialiasedLineEnable = TRUE;

				break;

			case RS_CULL_NONE:
				ZeroMemory(&rd, sizeof(rd));
				rd.FillMode = D3D11_FILL_SOLID;
				rd.CullMode = D3D11_CULL_NONE;
				rd.FrontCounterClockwise = FALSE;
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = TRUE;
				rd.ScissorEnable = FALSE;
				rd.MultisampleEnable = TRUE;
				rd.AntialiasedLineEnable = TRUE;

				break;

			case RS_CULL_SKY:
				ZeroMemory(&rd, sizeof(rd));
				rd.FillMode = D3D11_FILL_SOLID;
				rd.CullMode = D3D11_CULL_NONE;
				rd.FrontCounterClockwise = FALSE;
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = FALSE;
				rd.ScissorEnable = FALSE;
				rd.MultisampleEnable = TRUE;
				rd.AntialiasedLineEnable = FALSE;

				break;
		}
		HRESULT hr = Device->CreateRasterizerState(&rd, RasterizerState[state].GetAddressOf());
		//assert(FAILED(hr));

		if (FAILED(hr))
		{
			return false;
		}
	}

	return true;
}

//ブレンドステートの作成
bool DxSystem::CreateBlendState()
{
	D3D11_BLEND_DESC bd;

	for (int state = 0; state < BLEND_TYPE; state++)
	{
		switch (state)
		{
			case BS_NONE:
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

			case BS_ALPHA:
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

			case BS_ALPHA_TEST:
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

			case BS_TRANSPARENT:
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

			case BS_ADD:
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

			case BS_SUBTRACT:
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

			case BS_REPLACE:
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
			case BS_MULTIPLY:
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
			case BS_LIGHTEN:
				ZeroMemory(&bd, sizeof(bd));
				bd.IndependentBlendEnable = false;
				bd.AlphaToCoverageEnable = false;
				bd.RenderTarget[0].BlendEnable = true;
				bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;

				bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				break;

			case BS_DARKEN:
				ZeroMemory(&bd, sizeof(bd));
				bd.IndependentBlendEnable = false;
				bd.AlphaToCoverageEnable = false;
				bd.RenderTarget[0].BlendEnable = true;
				bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;

				bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				break;
			case BS_SCREEN:
				ZeroMemory(&bd, sizeof(bd));
				bd.IndependentBlendEnable = false;
				bd.AlphaToCoverageEnable = false;
				bd.RenderTarget[0].BlendEnable = true;
				bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
				bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				break;
		}
		//ブレンドステートの作成
		HRESULT hr = Device->CreateBlendState(&bd, BlendState[state].GetAddressOf());
		//assert(FAILED(hr));

		if (FAILED(hr))
		{
			return false;
		}
	}
	return true;
}

//------------------------------------------------
//	クリア
//------------------------------------------------
void DxSystem::Clear(DWORD color)
{
	// レンダーターゲットビュー設定
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
	DeviceContext->OMSetDepthStencilState(DepthStencilState[DS_TRUE].Get(), 1);
}

//------------------------------------------------
//	フリップ
//------------------------------------------------
void DxSystem::Flip(int n)
{
	// フリップ処理
	SwapChain->Present(n, 0);
}