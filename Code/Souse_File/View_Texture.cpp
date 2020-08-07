#include "View_Texture.h"
#include "DxSystem.h"
using namespace std;
using namespace DirectX;

View_Texture::View_Texture()
{
	CreateRenderTartgetView(800, 600);
	CreateDepthStencil(800, 600);

	// 定数バッファの生成
	if (!ConstantBuffer_CbScene)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CbScene);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::Device->CreateBuffer(&bd, nullptr, ConstantBuffer_CbScene.GetAddressOf());
		assert(SUCCEEDED(hr), hr_trace(hr));
	}
}

void View_Texture::Clear()
{
	float clearColor[4] = { 0,1,0,0 };
	DxSystem::DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), clearColor);
	DxSystem::DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void View_Texture::Set_Screen_Size(int x, int y)
{
	if (screen_x != x || screen_y != y)
	{

		CreateRenderTartgetView(x, y);
		CreateDepthStencil(x, y);

		screen_x = x;
		screen_y = y;
	}
}

bool View_Texture::CreateDepthStencil(int x, int y)
{
	// 深度ステンシル設定
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = x;
	td.Height = y;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R24G8_TYPELESS;
	//td.SampleDesc = DxSystem::MSAA;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	// 深度ステンシルテクスチャ生成
	HRESULT hr = DxSystem::Device->CreateTexture2D(&td, NULL, Texture_DepthStencil.GetAddressOf());
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
	hr = DxSystem::Device->CreateDepthStencilView(Texture_DepthStencil.Get(), &dsvd, DepthStencilView.GetAddressOf());
	//assert(FAILED(hr));

	if (FAILED(hr))
	{
		return false;
	}

	// シェーダリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	// シェーダリソースビュー生成
	hr = DxSystem::Device->CreateShaderResourceView(Texture_DepthStencil.Get(), &srvd, ShaderResourceView_Depth.GetAddressOf());
	//assert(FAILED(hr));

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool View_Texture::CreateRenderTartgetView(int x, int y)
{
	// 2次元テクスチャの設定
	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.Width = x;
	texDesc.Height = y;
	texDesc.CPUAccessFlags = 0;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	//texDesc.SampleDesc = DxSystem::MSAA;
	texDesc.SampleDesc.Count = 1;
	// 2次元テクスチャの生成
	HRESULT hr = DxSystem::Device->CreateTexture2D(&texDesc, NULL, Texture_RenderTarget.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// レンダーターゲットビューの設定
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset(&rtvDesc, 0, sizeof(rtvDesc));
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	// レンダーターゲットビューの生成
	hr = DxSystem::Device->CreateRenderTargetView(Texture_RenderTarget.Get(), &rtvDesc, RenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// シェーダリソースビューの設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;;

	// シェーダリソースビューの生成
	hr = DxSystem::Device->CreateShaderResourceView(Texture_RenderTarget.Get(), &srvDesc, ShaderResourceView_Render.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
}