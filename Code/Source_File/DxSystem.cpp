#include "DxSystem.h"
#include <DirectXMath.h>
#include "Misc.h"
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace BeastEngine;

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

ComPtr<ID3D11Device>		        DxSystem::device;
ComPtr<IDXGISwapChain>		        DxSystem::swap_chain;

ComPtr<ID3D11DeviceContext>		    DxSystem::device_context;
ComPtr<ID3D11RenderTargetView>      DxSystem::render_target_view;

ComPtr<ID3D11Texture2D>				DxSystem::depth_stencil_texture;
ComPtr<ID3D11DepthStencilView>		DxSystem::depth_stencil_view;
ComPtr<ID3D11ShaderResourceView>	DxSystem::shader_resource_view;
ComPtr<ID3D11DepthStencilState>		DxSystem::depth_stencil_state[16];
ComPtr<ID3D11RasterizerState>		DxSystem::rasterizer_state[rasterizer_type];
ComPtr<ID3D11BlendState>			DxSystem::blend_state[blend_type];

ComPtr<IDXGIDebug>                  DxSystem::dxgi_debug;
HWND								DxSystem::hwnd;
DXGI_SAMPLE_DESC					DxSystem::MSAA;
int DxSystem::screen_width = 1920;
int DxSystem::screen_height = 1080;

bool DxSystem::Initialize(HWND hWnd, int width, int height)
{
	hwnd = hWnd;
	Create_Device();
	Initialize_Render_Target();
	Create_Rasterizer_State();
	Create_Blend_State();
	return true;
}

HRESULT DxSystem::Create_Device()
{
	HRESULT hr;
	UINT creation_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	// デバッグレイヤーの設定
	//creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//デバイスの生成
	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creation_flags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		device.GetAddressOf(),
		nullptr,
		device_context.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxW(hwnd, L"D3D11CreateDevice", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

#if defined(_DEBUG)
	/*
	//デバッグモード開始
	if (DXGI_debug == nullptr)
	{
		// 作成
		typedef HRESULT(__stdcall* fPtr)(const IID&, void**);
		HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");
		fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");

		DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&DXGI_debug);

		// 出力
		DXGI_debug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
	}
	else
		DXGI_debug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
		*/
#endif
		///*
		//使用可能なMSAAを取得
	constexpr int max_count = 4;//D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
	for (int i = 0; i <= max_count; i++)
	{
		UINT Quality;
		if SUCCEEDED(device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, i, &Quality))
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
	IDXGIDevice1* hpDXGI = nullptr;
	if (FAILED(device.Get()->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(&hpDXGI))))
	{
		MessageBoxW(hwnd, L"QueryInterface", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	//アダプター取得
	IDXGIAdapter* hpAdapter = nullptr;
	if (FAILED(hpDXGI->GetAdapter(&hpAdapter)))
	{
		MessageBoxW(hwnd, L"GetAdapter", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	//ファクトリー取得
	IDXGIFactory* hpDXGIFactory = nullptr;
	hpAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&hpDXGIFactory));
	if (hpDXGIFactory == nullptr)
	{
		MessageBoxW(hwnd, L"GetParent", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	screen_width = clientRect.right;
	screen_height = clientRect.bottom;

	//スワップチェイン作成
	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferDesc.Width = screen_width;
	scd.BufferDesc.Height = screen_height;
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
	if (FAILED(hpDXGIFactory->CreateSwapChain(device.Get(), &scd, swap_chain.GetAddressOf())))
	{
		MessageBoxW(hwnd, L"CreateSwapChain", L"Err", MB_ICONSTOP);
		return S_FALSE;
	}

	return S_OK;
}

void DxSystem::Release()
{
}

bool DxSystem::Initialize_Render_Target()
{
	// バックバッファ取得
	ID3D11Texture2D* BackBuffer = nullptr;
	HRESULT hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&BackBuffer));

	if (FAILED(hr))
	{
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		return false;
	}

	// レンダーターゲットビュー生成
	hr = device->CreateRenderTargetView(BackBuffer, nullptr, render_target_view.GetAddressOf());
	BackBuffer->Release();
	BackBuffer = nullptr;

	if (FAILED(hr))
	{
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		return false;
	}
	//  深度ステンシルバッファ生成
	Create_Depth_Stencil();

	// ビューポートの設定
	Set_ViewPort(screen_width, screen_height);

	return true;
}

void DxSystem::Set_ViewPort(int width, int height, int Num)
{
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(width);
	vp.Height = static_cast<FLOAT>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	device_context->RSSetViewports(Num, &vp);
}

void DxSystem::Set_Default_View()
{
	device_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
	Set_ViewPort(screen_width, screen_height);
}

bool DxSystem::Create_Depth_Stencil()
{
	// 深度ステンシル設定
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = screen_width;
	td.Height = screen_height;
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

	// 深度ステンシルテクスチャ生成
	HRESULT hr = device->CreateTexture2D(&td, nullptr, &depth_stencil_texture);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvd.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー生成
	hr = device->CreateDepthStencilView(depth_stencil_texture.Get(), &dsvd, depth_stencil_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//デプスステンシルステート
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = FALSE;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::None)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::Less)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_GREATER;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::Greater)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::LEqual)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::GEqual)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::Equal)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::NotEqual)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::Always)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::None_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::Less_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_GREATER;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::Greater_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::LEqual_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::GEqual_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::Equal_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::NotEqual_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state[static_cast<int>(DS_State::Always_No_Write)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// シェーダリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	// シェーダリソースビュー生成
	hr = device->CreateShaderResourceView(depth_stencil_texture.Get(), &srvd, shader_resource_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool DxSystem::Create_Rasterizer_State()
{
	D3D11_RASTERIZER_DESC rd;
	for (int state = 0; state < rasterizer_type; ++state)
	{
		switch (static_cast<RS_State>(state))
		{
			case RS_State::Cull_Back:
				ZeroMemory(&rd, sizeof(rd));
				rd.FillMode = D3D11_FILL_SOLID;	 // 塗りつぶし
				rd.CullMode = D3D11_CULL_BACK;	 // カリング
				rd.FrontCounterClockwise = TRUE; // 三角形の時計回りが正面
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = TRUE;	     // 距離に基づくクリッピングを有効か
				rd.ScissorEnable = FALSE;	     // シザー長方形カリングを有効か
				rd.MultisampleEnable = TRUE;	 // MSAAで四辺形かアルファ線を設定
				rd.AntialiasedLineEnable = TRUE; // ラインAAが有効か
				break;

			case RS_State::Cull_Front:
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

			case RS_State::Cull_None:
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

			case RS_State::Standard:
				ZeroMemory(&rd, sizeof(rd));
				rd.FillMode = D3D11_FILL_SOLID;
				rd.CullMode = D3D11_CULL_NONE;
				rd.FrontCounterClockwise = TRUE;
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = TRUE;
				rd.ScissorEnable = FALSE;
				rd.MultisampleEnable = TRUE;
				rd.AntialiasedLineEnable = TRUE;
				break;

			case RS_State::Wire:
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
		const HRESULT hr = device->CreateRasterizerState(&rd, rasterizer_state[state].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	return true;
}

bool DxSystem::Create_Blend_State()
{
	D3D11_BLEND_DESC bd;

	for (int state = 0; state < blend_type; state++)
	{
		switch (static_cast<BS_State>(state))
		{
			case BS_State::Off:
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

			case BS_State::Alpha:
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

			case BS_State::Alpha_Test:
				ZeroMemory(&bd, sizeof(bd));
				bd.IndependentBlendEnable = false;
				bd.AlphaToCoverageEnable = true;
				bd.RenderTarget[0].BlendEnable = true;
				bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

				bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
				//bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				break;

			case BS_State::Transparent:
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

			case BS_State::Add:
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

			case BS_State::Subtract:
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

			case BS_State::Replace:
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
			case BS_State::Multiply:
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
		//ブレンドステートの作成
		const HRESULT hr = device->CreateBlendState(&bd, blend_state[state].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	return true;
}

void DxSystem::Clear(DWORD color)
{
	// レンダーターゲットビュー設定
	device_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

	constexpr float clear_color[4] = { 1,0,0,0 };
	/*
	for (int i = 3; i >= 0; i--)
	{
		clear_color[i] = ((color >> 8 * (3 - i)) & 0x00000000) / 255.0f;
	}
	*/
	device_context->ClearRenderTargetView(render_target_view.Get(), clear_color);
	device_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
	device_context->OMSetDepthStencilState(depth_stencil_state[static_cast<int>(DS_State::LEqual)].Get(), 1);
}

void DxSystem::Flip(int n)
{
	// フリップ処理
	swap_chain->Present(n, 0);
}