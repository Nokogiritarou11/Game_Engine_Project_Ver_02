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

	if (!shader)
	{
		shader = make_shared<Shader>();
		shader->Create_VS(L"Shader\\ShadowMap_Shader_VS.hlsl", "VSMain");
		// 深度ステンシル設定
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
		td.Width = Shadow_Map_Texture_Size;
		td.Height = Shadow_Map_Texture_Size;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R24G8_TYPELESS;
		td.SampleDesc = DxSystem::MSAA;
		//td.SampleDesc.Count = 1;
		//td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;

		// 深度ステンシルテクスチャ生成
		HRESULT hr = DxSystem::Device->CreateTexture2D(&td, NULL, DepthStencilTexture.GetAddressOf());
		assert(SUCCEEDED(hr));

		// 深度ステンシルビュー設定
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsvd.Texture2D.MipSlice = 0;

		// 深度ステンシルビュー生成
		hr = DxSystem::Device->CreateDepthStencilView(DepthStencilTexture.Get(), &dsvd, DepthStencilView.GetAddressOf());
		assert(SUCCEEDED(hr));

		// シェーダリソースビュー設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;

		// シェーダリソースビュー生成
		hr = DxSystem::Device->CreateShaderResourceView(DepthStencilTexture.Get(), &srvd, ShaderResourceView.GetAddressOf());
		assert(SUCCEEDED(hr));

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
		//*/
		/*
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		*/
		hr = DxSystem::Device->CreateSamplerState(
			&sd, sampler.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
}

void Light::Set(shared_ptr<Transform> trans)
{
	shader->Activate();
	DxSystem::DeviceContext->OMSetRenderTargets(0, NULL, DepthStencilView.Get());
	DxSystem::DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DxSystem::DeviceContext->RSSetState(DxSystem::GetRasterizerState(DxSystem::RS_CULL_BACK));
	//ブレンドステート設定
	DxSystem::DeviceContext->OMSetBlendState(DxSystem::GetBlendState(DxSystem::BS_NONE), nullptr, 0xFFFFFFFF);
	//デプスステンシルステート設定
	DxSystem::DeviceContext->OMSetDepthStencilState(DxSystem::GetDephtStencilState(DxSystem::DS_TRUE), 1);
	// ビューポートの設定
	DxSystem::SetViewPort(Shadow_Map_Texture_Size, Shadow_Map_Texture_Size);

	const Vector3 forward = transform->Get_forward();
	Direction = Vector4(forward.x, forward.y, forward.z, 0);

	// プロジェクション行列を作成
	{
		XMStoreFloat4x4(&P, XMMatrixOrthographicLH(Light_View_Size, Light_View_Size, 0.f, 1000.0f));
		//角度をラジアン(θ)に変換
		//float fov_y = XMConvertToRadians(30);	// 画角
		//float aspect = 1;	// 画面比率

		//XMStoreFloat4x4(&P, XMMatrixPerspectiveFovLH(fov_y, aspect, 0.1f, 1000.f));
	}
	// ビュー行列を作成
	// カメラの設定
	{
		Vector3 pos = -forward * Distance;
		Vector4 p = { pos.x, pos.y, pos.z, 0 };
		XMVECTOR eye_v = p;

		//XMVECTOR focus_v = eye_v + XMLoadFloat3(&forward);
		XMVECTOR focus_v = XMVectorSet(0, 0, 0, 0);

		//XMFLOAT4 u = { 0, 1, 0, 0 };
		//XMVECTOR up_v = XMLoadFloat4(&u);
		XMVECTOR up_v = XMVectorSet(0, 1, 0, 0);
		XMStoreFloat4x4(&V, XMMatrixLookAtLH(eye_v, focus_v, up_v));
	}
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
		ImGui::DragFloat(u8"距離", &Distance, 0.1f, 0.00001f, FLT_MAX);
		ImGui::DragFloat(u8"強度", &Intensity, 0.01f, 0, FLT_MAX);
		ImGui::DragFloat(u8"バイアス", &Bias, 0.0001f, -FLT_MAX, FLT_MAX, "%.4f");
		ImGui::DragFloat(u8"ライトビューサイズ", &Light_View_Size, 0.1f, 0.01f, FLT_MAX);
	}
	return true;
}