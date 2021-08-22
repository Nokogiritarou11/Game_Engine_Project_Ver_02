#include "Shadow_Manager.h"
#include "Gaussian_Filter.h"
#include "Material.h"
#include "Texture.h"
#include "Render_Texture.h"
using namespace BeastEngine;
using namespace DirectX;
using namespace std;

Shadow_Manager::Shadow_Manager()
{
	shadow_map_texture_size = 2048;
	Set_Shadow_Map_Texture_Size(shadow_map_texture_size);
	gaussian_filter = make_unique<Gaussian_Filter>(Vector2(static_cast<float>(shadow_map_texture_size), static_cast<float>(shadow_map_texture_size)), DXGI_FORMAT_R32G32_FLOAT, 0.1f);

	//マテリアル
	material_shadow = Material::Create("Shader/Standard_Shadow_Shader_VS.hlsl", "Shader/Standard_Shadow_Shader_PS.hlsl");
	material_shadow->Set_Blend_State(BS_State::Alpha_Test);
	material_shadow->Set_Rasterizer_State(RS_State::Cull_Back);
	material_shadow->Set_Depth_Stencil_State(DS_State::GEqual);
	material_shadow->Set_Texture("alphaMap", Texture::Load("Default_Resource\\Image\\Default_Texture.png"));

	//	サンプラステート作成
	float boarderColor[4] = { -FLT_MAX,-FLT_MAX,-FLT_MAX,-FLT_MAX };
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER; // U
	sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER; // V
	sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER; // W
	sd.MipLODBias = 0;
	sd.MaxAnisotropy = 8; // 最大異方性(1Pixelあたりのテクスチャ点数)
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
	material_shadow->Active_Shader();
	material_shadow->Active_State();
}

void Shadow_Manager::Set_Default_Shadow_Alpha()
{
	material_shadow->Active_Texture();
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
	render_texture = make_unique<Render_Texture>(shadow_map_texture_size, shadow_map_texture_size, true, DXGI_FORMAT_R32G32_FLOAT);
}