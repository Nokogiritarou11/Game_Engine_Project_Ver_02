#include "Shadow_Manager.h"
using namespace BeastEngine;
using namespace DirectX;
using namespace std;

Shadow_Manager::Shadow_Manager()
{
	if (!shader_resource_view)
	{
		// 深度ステンシル設定
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
		td.Width = shadow_map_texture_size;
		td.Height = shadow_map_texture_size;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R32G8X24_TYPELESS;
		//td.SampleDesc = DxSystem::MSAA;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;

		// 深度ステンシルテクスチャ生成
		HRESULT hr = DxSystem::device->CreateTexture2D(&td, NULL, depth_stencil_texture.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// 深度ステンシルビュー設定
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvd.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		//dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0;

		// 深度ステンシルビュー生成
		hr = DxSystem::device->CreateDepthStencilView(depth_stencil_texture.Get(), &dsvd, depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// シェーダリソースビュー設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvd.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		//srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;

		// シェーダリソースビュー生成
		hr = DxSystem::device->CreateShaderResourceView(depth_stencil_texture.Get(), &srvd, shader_resource_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//	サンプラステート作成
		D3D11_SAMPLER_DESC sd = {};
		///*
		sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.BorderColor[0] = 1.0f;
		sd.BorderColor[1] = 1.0f;
		sd.BorderColor[2] = 1.0f;
		sd.BorderColor[3] = 1.0f;
		sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		sd.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		sd.MaxAnisotropy = 1;
		sd.MipLODBias = 0;
		sd.MinLOD = -FLT_MAX;
		sd.MaxLOD = +FLT_MAX;

		hr = DxSystem::device->CreateSamplerState(
			&sd, sampler.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

Shadow_Manager::~Shadow_Manager()
{
}

void Shadow_Manager::Set_Shadow_Map_Texture()
{
	DxSystem::device_context->OMSetRenderTargets(0, NULL, depth_stencil_view.Get());
	DxSystem::device_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ビューポートの設定
	DxSystem::Set_ViewPort(shadow_map_texture_size, shadow_map_texture_size);
}

void Shadow_Manager::Set_PS_Resource()
{
	DxSystem::device_context->PSSetSamplers(0, 1, sampler.GetAddressOf());
	DxSystem::device_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
}

void Shadow_Manager::Set_Shadow_Map_Texture_Size(u_int size)
{
	shadow_map_texture_size = size;
	depth_stencil_texture.Reset();
	depth_stencil_view.Reset();
	shader_resource_view.Reset();

	// 深度ステンシル設定
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = shadow_map_texture_size;
	td.Height = shadow_map_texture_size;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R32G8X24_TYPELESS;
	//td.SampleDesc = DxSystem::MSAA;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	// 深度ステンシルテクスチャ生成
	HRESULT hr = DxSystem::device->CreateTexture2D(&td, NULL, depth_stencil_texture.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	//dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー生成
	hr = DxSystem::device->CreateDepthStencilView(depth_stencil_texture.Get(), &dsvd, depth_stencil_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// シェーダリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	//srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	// シェーダリソースビュー生成
	hr = DxSystem::device->CreateShaderResourceView(depth_stencil_texture.Get(), &srvd, shader_resource_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}