#include "Light.h"
#include "Light_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Include_ImGui.h"
#include "Debug_UI.h"
using namespace std;

Light::Light()
{
}
Light::~Light()
{
}

void Light::Initialize()
{
}
void Light::Initialize(std::shared_ptr<GameObject> obj)
{
	Light_Manager::Add(static_pointer_cast<Light>(shared_from_this()));

	gameObject = obj;
	transform = obj->transform;

	shader = make_unique<Shader>();
	shader->Create_VS(L"Shader\\ShadoeoMap_Shader_VS.hlsl", "VSMain");
	//shader->Create_PS(L"Shader\\ShadoeoMap_Shader_PS.hlsl", "PSMain");

	// 深度ステンシル設定
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = Shadow_Map_Texture_Size;
	td.Height = Shadow_Map_Texture_Size;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc = DxSystem::MSAA;
	//td.SampleDesc.Count = 1;
	//td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	// 深度ステンシルテクスチャ生成
	HRESULT hr = DxSystem::Device->CreateTexture2D(&td, NULL, &DepthStencilTexture);
	assert(FAILED(hr));

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D16_UNORM;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvd.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー生成
	hr = DxSystem::Device->CreateDepthStencilView(DepthStencilTexture.Get(), &dsvd, DepthStencilView.GetAddressOf());
	assert(FAILED(hr));

	// シェーダリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	// シェーダリソースビュー生成
	hr = DxSystem::Device->CreateShaderResourceView(DepthStencilTexture.Get(), &srvd, ShaderResourceView.GetAddressOf());
	//assert(FAILED(hr));

	//	サンプラステート作成
	D3D11_SAMPLER_DESC sd = {};
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
	hr = DxSystem::Device->CreateSamplerState(
		&sd, sampler.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void Light::Set()
{
	DxSystem::DeviceContext->OMSetRenderTargets(0, nullptr, DepthStencilView.Get());
	DxSystem::DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	shader->Activate();
	// ビューポートの設定
	DxSystem::SetViewPort(Shadow_Map_Texture_Size, Shadow_Map_Texture_Size);
}

bool Light::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Light"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Light_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<SkinMesh_Renderer>(shared_from_this()));
				removed = false;
			}
			ImGui::EndPopup();
		}
		if (!removed)
		{
			return false;
		}

		float out_color[4] = { Color.x,Color.y,Color.z,Color.w };
		ImGui::ColorEdit3("Color", out_color);
		Color = { out_color[0],out_color[1] ,out_color[2] ,out_color[3] };
		ImGui::DragFloat(u8"強度", &Intensity, 0.01f, 0, FLT_MAX);
		ImGui::DragFloat(u8"バイアス", &Bias, 0.001f, -FLT_MAX, FLT_MAX);
	}
	return true;
}