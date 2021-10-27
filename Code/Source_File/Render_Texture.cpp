#include "Render_Texture.h"
#include "DxSystem.h"
#include "Misc.h"
#include "Engine.h"
#include "Particle_Manager.h"
using namespace BeastEngine;
using namespace std;
using Microsoft::WRL::ComPtr;

Render_Texture::Render_Texture(const int& x, const int& y, const bool& msaa, const DXGI_FORMAT& format)
{
	use_msaa = msaa;
	color_format = format;
	Set_Screen_Size(x, y);
}

Render_Texture::~Render_Texture()
{
	if (back_ground_texture != nullptr)
	{
		back_ground_texture.Reset();
		back_ground_texture = nullptr;
	}
}

void Render_Texture::Clear() const
{
	constexpr float clear_color[4] = { 0,0,0,0 };
	DxSystem::device_context->ClearRenderTargetView(render_target_view.Get(), clear_color);
	DxSystem::device_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0);
}

void Render_Texture::Set_Screen_Size(const int& x, const int& y)
{
	if (screen_x != x || screen_y != y)
	{
		screen_x = x;
		screen_y = y;

		Create_Render_Target_View();
		Create_Depth_Stencil();
		back_ground_texture = EffekseerRendererDX11::CreateTexture(Engine::particle_manager->renderer->GetGraphicsDevice(), shader_resource_view_copy.Get(), nullptr, nullptr);
	}
}

void Render_Texture::Set_Render_Target()
{
	Clear();
	// ビューポートの設定
	DxSystem::Set_ViewPort(screen_x, screen_y);
	DxSystem::device_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
}

void Render_Texture::Update_Copy_Texture() const
{
	DxSystem::device_context->CopyResource(texture_render_copy.Get(),texture_render_target.Get());
}


bool Render_Texture::Create_Depth_Stencil()
{
	// 深度ステンシル設定
	D3D11_TEXTURE2D_DESC td = {};
	td.Width = screen_x;
	td.Height = screen_y;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R32G8X24_TYPELESS;
	if (use_msaa)
	{
		td.SampleDesc = DxSystem::MSAA;
	}
	else
	{
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
	}
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	// 深度ステンシルテクスチャ生成
	HRESULT hr = DxSystem::device->CreateTexture2D(&td, nullptr, texture_depth_stencil.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	if (use_msaa)
	{
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	}
	dsvd.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー生成
	hr = DxSystem::device->CreateDepthStencilView(texture_depth_stencil.Get(), &dsvd, depth_stencil_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// シェーダリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	if (use_msaa)
	{
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	}
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	// シェーダリソースビュー生成
	hr = DxSystem::device->CreateShaderResourceView(texture_depth_stencil.Get(), &srvd, shader_resource_view_depth.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return true;
}

bool Render_Texture::Create_Render_Target_View()
{
	{
		// 2次元テクスチャの設定
		D3D11_TEXTURE2D_DESC tex_desc = {};
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.Format = color_format;
		tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex_desc.Width = screen_x;
		tex_desc.Height = screen_y;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		if (use_msaa)
		{
			tex_desc.SampleDesc = DxSystem::MSAA;
		}
		else
		{
			tex_desc.SampleDesc.Count = 1;
			tex_desc.SampleDesc.Quality = 0;
		}
		// 2次元テクスチャの生成
		HRESULT hr = DxSystem::device->CreateTexture2D(&tex_desc, nullptr, texture_render_target.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// レンダーターゲットビューの設定
		D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
		rtv_desc.Format = tex_desc.Format;
		if (use_msaa)
		{
			rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		}
		rtv_desc.Texture2D.MipSlice = 0;

		// レンダーターゲットビューの生成
		hr = DxSystem::device->CreateRenderTargetView(texture_render_target.Get(), &rtv_desc, render_target_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// シェーダリソースビューの設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = tex_desc.Format;
		if (use_msaa)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		// シェーダリソースビューの生成
		hr = DxSystem::device->CreateShaderResourceView(texture_render_target.Get(), &srvDesc, shader_resource_view_render.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		// コピー用テクスチャの設定
		D3D11_TEXTURE2D_DESC tex_desc = {};
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.Format = color_format;
		tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex_desc.Width = screen_x;
		tex_desc.Height = screen_y;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		if (use_msaa)
		{
			tex_desc.SampleDesc = DxSystem::MSAA;
		}
		else
		{
			tex_desc.SampleDesc.Count = 1;
			tex_desc.SampleDesc.Quality = 0;
		}
		// コピー用テクスチャの生成
		HRESULT hr = DxSystem::device->CreateTexture2D(&tex_desc, nullptr, texture_render_copy.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// コピー用シェーダリソースビューの設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = tex_desc.Format;
		if (use_msaa)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		// コピー用シェーダリソースビューの生成
		hr = DxSystem::device->CreateShaderResourceView(texture_render_copy.Get(), &srvDesc, shader_resource_view_copy.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	if (use_msaa)
	{
		// 2次元テクスチャの設定
		D3D11_TEXTURE2D_DESC tex_desc = {};
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.Format = color_format;
		tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex_desc.Width = screen_x;
		tex_desc.Height = screen_y;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		// 2次元テクスチャの生成
		HRESULT hr = DxSystem::device->CreateTexture2D(&tex_desc, nullptr, texture_render_resolve.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Format = color_format;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = 1;
		srv_desc.Texture2D.MostDetailedMip = 0;
		// シェーダリソースビューの生成
		hr = DxSystem::device->CreateShaderResourceView(texture_render_resolve.Get(), &srv_desc, shader_resource_view_resolve.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	return true;
}

ComPtr<ID3D11ShaderResourceView>& Render_Texture::Get_Texture()
{
	if (use_msaa)
	{
		DxSystem::device_context->ResolveSubresource(texture_render_resolve.Get(), 0, texture_render_target.Get(), 0, color_format);
		return shader_resource_view_resolve;
	}

	return shader_resource_view_render;
}