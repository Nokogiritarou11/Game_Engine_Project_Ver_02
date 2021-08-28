#include "Render_Texture.h"
#include "DxSystem.h"
using namespace BeastEngine;
using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;

Render_Texture::Render_Texture(const int& x, const int& y, const bool& MSAA, const DXGI_FORMAT& format)
{
	use_msaa = MSAA;
	color_format = format;
	Set_Screen_Size(x, y);
}

void Render_Texture::Clear()
{
	float clearColor[4] = { 0,0,0,0 };
	DxSystem::device_context->ClearRenderTargetView(render_target_view.Get(), clearColor);
	DxSystem::device_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0);
}

void Render_Texture::Set_Screen_Size(const int& x, const int& y)
{
	if (screen_x != x || screen_y != y)
	{
		screen_x = x;
		screen_y = y;

		Create_Render_Tartget_View();
		Create_Depth_Stencil();
	}
}

void Render_Texture::Set_Render_Target()
{
	Clear();
	// �r���[�|�[�g�̐ݒ�
	DxSystem::Set_ViewPort(screen_x, screen_y);
	DxSystem::device_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
}

bool Render_Texture::Create_Depth_Stencil()
{
	// �[�x�X�e���V���ݒ�
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
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

	// �[�x�X�e���V���e�N�X�`������
	HRESULT hr = DxSystem::device->CreateTexture2D(&td, nullptr, texture_depth_stencil.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �[�x�X�e���V���r���[�ݒ�
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
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

	// �[�x�X�e���V���r���[����
	hr = DxSystem::device->CreateDepthStencilView(texture_depth_stencil.Get(), &dsvd, depth_stencil_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �V�F�[�_���\�[�X�r���[�ݒ�
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
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

	// �V�F�[�_���\�[�X�r���[����
	hr = DxSystem::device->CreateShaderResourceView(texture_depth_stencil.Get(), &srvd, shader_resource_view_depth.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return true;
}

bool Render_Texture::Create_Render_Tartget_View()
{
	{
		// 2�����e�N�X�`���̐ݒ�
		D3D11_TEXTURE2D_DESC texDesc;
		memset(&texDesc, 0, sizeof(texDesc));
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.Format = color_format;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.Width = screen_x;
		texDesc.Height = screen_y;
		texDesc.CPUAccessFlags = 0;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		if (use_msaa)
		{
			texDesc.SampleDesc = DxSystem::MSAA;
		}
		else
		{
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
		}
		// 2�����e�N�X�`���̐���
		HRESULT hr = DxSystem::device->CreateTexture2D(&texDesc, nullptr, texture_render_target.GetAddressOf());

		// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		memset(&rtvDesc, 0, sizeof(rtvDesc));
		rtvDesc.Format = texDesc.Format;
		if (use_msaa)
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		}
		rtvDesc.Texture2D.MipSlice = 0;

		// �����_�[�^�[�Q�b�g�r���[�̐���
		hr = DxSystem::device->CreateRenderTargetView(texture_render_target.Get(), &rtvDesc, render_target_view.GetAddressOf());

		// �V�F�[�_���\�[�X�r���[�̐ݒ�
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(srvDesc));
		srvDesc.Format = texDesc.Format;
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

		// �V�F�[�_���\�[�X�r���[�̐���
		hr = DxSystem::device->CreateShaderResourceView(texture_render_target.Get(), &srvDesc, shader_resource_view_render.GetAddressOf());
	}

	if (use_msaa)
	{
		// 2�����e�N�X�`���̐ݒ�
		D3D11_TEXTURE2D_DESC texDesc;
		memset(&texDesc, 0, sizeof(texDesc));
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.Format = color_format;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.Width = screen_x;
		texDesc.Height = screen_y;
		texDesc.CPUAccessFlags = 0;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		// 2�����e�N�X�`���̐���
		HRESULT hr = DxSystem::device->CreateTexture2D(&texDesc, nullptr, texture_render_resolve.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(srvDesc));
		srvDesc.Format = color_format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		// �V�F�[�_���\�[�X�r���[�̐���
		hr = DxSystem::device->CreateShaderResourceView(texture_render_resolve.Get(), &srvDesc, shader_resource_view_resolve.GetAddressOf());
	}

	return true;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& Render_Texture::Get_Texture()
{
	if (use_msaa)
	{
		DxSystem::device_context->ResolveSubresource(texture_render_resolve.Get(), 0, texture_render_target.Get(), 0, color_format);
		return shader_resource_view_resolve;
	}
	else
	{
		return shader_resource_view_render;
	}

	return shader_resource_view_render;
}