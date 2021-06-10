#include "Light.h"
#include "Engine.h"
#include "Light_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Include_ImGui.h"
#include "Editor_UI.h"
using namespace std;

void Light::Initialize(std::shared_ptr<GameObject> obj)
{
	Engine::light_manager->Add(static_pointer_cast<Light>(shared_from_this()));

	gameObject = obj;
	transform = obj->transform;

	if (!ShaderResourceView)
	{
		// 深度ステンシル設定
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
		td.Width = Shadow_Map_Texture_Size;
		td.Height = Shadow_Map_Texture_Size;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R32G8X24_TYPELESS;
		td.SampleDesc = DxSystem::MSAA;
		//td.SampleDesc.Count = 1;
		//td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;

		// 深度ステンシルテクスチャ生成
		HRESULT hr = DxSystem::Device->CreateTexture2D(&td, NULL, DepthStencilTexture.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// 深度ステンシルビュー設定
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvd.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		//dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0;

		// 深度ステンシルビュー生成
		hr = DxSystem::Device->CreateDepthStencilView(DepthStencilTexture.Get(), &dsvd, DepthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// シェーダリソースビュー設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvd.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		//srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;

		// シェーダリソースビュー生成
		hr = DxSystem::Device->CreateShaderResourceView(DepthStencilTexture.Get(), &srvd, ShaderResourceView.GetAddressOf());
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

		hr = DxSystem::Device->CreateSamplerState(
			&sd, sampler.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void Light::Set(shared_ptr<Transform> trans)
{
	DxSystem::DeviceContext->OMSetRenderTargets(0, NULL, DepthStencilView.Get());
	DxSystem::DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DxSystem::DeviceContext->RSSetState(DxSystem::GetRasterizerState(RS_State::Cull_Front));
	//ブレンドステート設定
	DxSystem::DeviceContext->OMSetBlendState(DxSystem::GetBlendState(BS_State::Off), nullptr, 0xFFFFFFFF);
	//デプスステンシルステート設定
	DxSystem::DeviceContext->OMSetDepthStencilState(DxSystem::GetDephtStencilState(DS_State::LEqual), 1);
	// ビューポートの設定
	DxSystem::SetViewPort(Shadow_Map_Texture_Size, Shadow_Map_Texture_Size);

	const Vector3 forward = transform->Get_forward();
	Direction = Vector4(forward.x, forward.y, forward.z, 0);

	// プロジェクション行列を作成
	{
		if (Shadow_Distance <= 0)
		{
			Shadow_Distance = 0.1f;
		}
		//角度をラジアン(θ)に変換
		//float fov_y = XMConvertToRadians(30);	// 画角
		//float aspect = 1;	// 画面比率

		//XMStoreFloat4x4(&P, XMMatrixPerspectiveFovLH(fov_y, aspect, 0.1f, 1000.f));
	}
	// ビュー行列を作成
	// カメラの設定
	{
		//Vector3 Look_pos = trans->Get_position() + trans->Get_forward() * Shadow_Distance * 0.4f;
		Vector3 Look_pos = trans->Get_position();

		//Vector3 pos = Look_pos + (-forward * Distance);
		//Vector4 p = { pos.x, pos.y, pos.z, 0 };
		//XMVECTOR eye_v = p;

		//XMVECTOR focus_v = eye_v + XMLoadFloat3(&forward);
		//Vector3 focus = Look_pos;
		//XMVECTOR focus_v = XMVectorSet(0, 0, 0, 0);

		//XMFLOAT4 u = { 0, 1, 0, 0 };
		//XMVECTOR up_v = XMLoadFloat4(&u);
		//XMVECTOR up_v = XMVectorSet(0, 1, 0, 0);
		Look_pos = Look_pos - (forward * 10.0f);
		V = XMMatrixLookAtRH(Look_pos, Look_pos + forward, transform->Get_up());
	}
}

bool Light::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_AllowItemOverlap);

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

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
	static bool enable;
	enable = enableSelf();
	if (ImGui::Checkbox("##enable", &enable))
	{
		SetEnabled(enable);
	}

	if (open)
	{
		float out_color[4] = { Color.x,Color.y,Color.z,Color.w };
		ImGui::ColorEdit3("Color", out_color);
		Color = { out_color[0],out_color[1] ,out_color[2] ,out_color[3] };
		ImGui::DragFloat(u8"強度", &Intensity, 0.01f, 0, FLT_MAX);
		ImGui::DragFloat(u8"バイアス", &Bias, 0.000001f, -FLT_MAX, FLT_MAX, "%.6f");
		ImGui::DragFloat(u8"シャドウ描画距離", &Shadow_Distance, 0.1f, 0.01f, FLT_MAX);
	}
	return true;
}