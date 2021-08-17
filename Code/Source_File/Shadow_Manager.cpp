#include "Shadow_Manager.h"
#include "Gaussian_Filter.h"
#include "Material.h"
#include "Render_Texture.h"
using namespace BeastEngine;
using namespace DirectX;
using namespace std;

Shadow_Manager::Shadow_Manager()
{
	Set_Shadow_Map_Texture_Size(shadow_map_texture_size);
	material_shadow = Material::Create("Shader/Standard_Shadow_Shader_VS.hlsl", "Shader/Standard_Shadow_Shader_PS.hlsl");
	gaussian_filter = make_unique<Gaussian_Filter>(Vector2(static_cast<float>(shadow_map_texture_size), static_cast<float>(shadow_map_texture_size)), DXGI_FORMAT_R32G32_FLOAT, 0.1f);

	//	サンプラステート作成
	float boarderColor[4] = {0,0,0,0 };
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER; // U
	sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER; // V
	sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER; // W
	sd.MipLODBias = 0;
	sd.MaxAnisotropy = 16; // 最大異方性(1Pixelあたりのテクスチャ点数)
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	memcpy(sd.BorderColor, &boarderColor[0], sizeof(float) * 4);
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	auto hr = DxSystem::device->CreateSamplerState(&sd, sampler_shadow.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Shadow_Manager::Set_Shadow_Map_Texture()
{
	render_texture->Set_Render_Target();
	material_shadow->Active();
}

void Shadow_Manager::Set_PS_Resource()
{
	DxSystem::device_context->PSSetSamplers(0, 1, sampler_shadow.GetAddressOf());
	DxSystem::device_context->PSSetShaderResources(0, 1, gaussian_filter->Get_Texture().GetAddressOf());
}

void Shadow_Manager::Filtering_Gaussian()
{
	gaussian_filter->Filtering_Gaussian(render_texture->Get_Texture());
}

void Shadow_Manager::Set_Shadow_Map_Texture_Size(u_int size)
{
	shadow_map_texture_size = size;
	render_texture = make_unique<Render_Texture>(shadow_map_texture_size, shadow_map_texture_size, false, DXGI_FORMAT_R32G32_FLOAT);
}