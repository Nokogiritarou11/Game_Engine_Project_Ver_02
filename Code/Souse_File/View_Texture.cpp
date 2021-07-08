#include "View_Texture.h"
#include "DxSystem.h"
#include "Light_Manager.h"
#include "Engine.h"
#include "Render_Manager.h"
#include "Shadow_Manager.h"
#include "SkyBox.h"
#include "Transform.h"
#include "Renderer.h"
#include "Light.h"
using namespace BeastEngine;
using namespace std;
using namespace DirectX;

View_Texture::View_Texture()
{
	Create_Render_Tartget_View(800, 600);
	Create_Depth_Stencil(800, 600);

	HRESULT hr;
	// 定数バッファの生成
	if (!constant_buffer_scene)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(Constant_Buffer_Scene);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		hr = DxSystem::device->CreateBuffer(&bd, nullptr, constant_buffer_scene.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	if (!skybox)
	{
		skybox = make_unique<SkyBox>();
	}
}

void View_Texture::Clear()
{
	float clearColor[4] = { 0,1,0,0 };
	DxSystem::device_context->ClearRenderTargetView(render_target_view.Get(), clearColor);
	DxSystem::device_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void View_Texture::Set_Screen_Size(int x, int y)
{
	if (screen_x != x || screen_y != y)
	{

		Create_Render_Tartget_View(x, y);
		Create_Depth_Stencil(x, y);

		screen_x = x;
		screen_y = y;
	}
}

bool View_Texture::Create_Depth_Stencil(int x, int y)
{
	// 深度ステンシル設定
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = x;
	td.Height = y;
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
	HRESULT hr = DxSystem::device->CreateTexture2D(&td, NULL, texture_depth_stencil.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvd.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー生成
	hr = DxSystem::device->CreateDepthStencilView(texture_depth_stencil.Get(), &dsvd, depth_stencil_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// シェーダリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	// シェーダリソースビュー生成
	hr = DxSystem::device->CreateShaderResourceView(texture_depth_stencil.Get(), &srvd, shader_resource_view_depth.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return true;
}

bool View_Texture::Create_Render_Tartget_View(int x, int y)
{
	// 2次元テクスチャの設定
	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.Width = x;
	texDesc.Height = y;
	texDesc.CPUAccessFlags = 0;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc = DxSystem::MSAA;
	//texDesc.SampleDesc.Count = 1;
	// 2次元テクスチャの生成
	HRESULT hr = DxSystem::device->CreateTexture2D(&texDesc, NULL, texture_render_target.GetAddressOf());

	// レンダーターゲットビューの設定
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset(&rtvDesc, 0, sizeof(rtvDesc));
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	rtvDesc.Texture2D.MipSlice = 0;

	// レンダーターゲットビューの生成
	hr = DxSystem::device->CreateRenderTargetView(texture_render_target.Get(), &rtvDesc, render_target_view.GetAddressOf());

	// シェーダリソースビューの設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	// シェーダリソースビューの生成
	hr = DxSystem::device->CreateShaderResourceView(texture_render_target.Get(), &srvDesc, shader_resource_view_render.GetAddressOf());

	return true;
}

void View_Texture::Render_Sky(Vector3 pos)
{
	//トポロジー設定
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//ブレンドステート設定
	DxSystem::device_context->OMSetBlendState(DxSystem::Get_Blend_State(BS_State::Off), nullptr, 0xFFFFFFFF);
	Renderer::binding_blend_state = BS_State::Off;
	//ラスタライザ―設定
	DxSystem::device_context->RSSetState(DxSystem::Get_Rasterizer_State(RS_State::Cull_None));
	Renderer::binding_rasterizer_state = RS_State::Cull_None;
	//デプスステンシルステート設定
	DxSystem::device_context->OMSetDepthStencilState(DxSystem::Get_DephtStencil_State(DS_State::None_No_Write), 1);
	Renderer::binding_depth_stencil_State = DS_State::None_No_Write;

	skybox->Render(pos);
}

void View_Texture::Render_Shadow(std::shared_ptr<Transform> camera_transform)
{
	for (vector<weak_ptr<Light>>::iterator itr = Engine::light_manager->light_list.begin(); itr != Engine::light_manager->light_list.end();)
	{
		if (itr->expired())
		{
			itr = Engine::light_manager->light_list.erase(itr);
			continue;
		}
		shared_ptr<Light> m_light = itr->lock();
		if (m_light->gameobject->Get_Active_In_Hierarchy())
		{
			if (m_light->Get_Enabled())
			{
				Render_Shadow_Directional(m_light->color, m_light->intensity, m_light->transform, camera_transform);
			}
		}
		++itr;
	}
}

void View_Texture::Render_Shadow_Directional(Vector4 color, float intensity, std::shared_ptr<Transform> light_transform, std::shared_ptr<Transform> camera_transform)
{
	Matrix V, P, VP;
	Vector3 Look_pos = camera_transform->Get_Position();
	V = XMMatrixLookAtRH(Look_pos - light_transform->Get_Forward(), Look_pos, light_transform->Get_Up());
	float size = (float)Engine::shadow_manager->Get_Shadow_Map_Texture_Size();
	P = XMMatrixOrthographicRH(size, size, 0.1f, 100.f);
	VP = V * P;

	buffer_scene.view_projection_matrix = VP;
	static const Matrix SHADOW_BIAS = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f };
	buffer_scene.shadow_matrix = VP * SHADOW_BIAS;
	const Vector3 forward = light_transform->Get_Forward();
	buffer_scene.light_direction = { forward.x, forward.y, forward.z, 0 };
	buffer_scene.light_color = { color.x, color.y, color.z };
	buffer_scene.bias = Engine::shadow_manager->Get_Shadow_Bias();

	DxSystem::device_context->VSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
	DxSystem::device_context->UpdateSubresource(constant_buffer_scene.Get(), 0, 0, &buffer_scene, 0, 0);

	Engine::shadow_manager->Set_Shadow_Map_Texture();
	//トポロジー設定
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//ブレンドステート設定
	DxSystem::device_context->OMSetBlendState(DxSystem::Get_Blend_State(BS_State::Off), nullptr, 0xFFFFFFFF);
	Renderer::binding_blend_state = BS_State::Off;
	//ラスタライザ―設定
	DxSystem::device_context->RSSetState(DxSystem::Get_Rasterizer_State(RS_State::Cull_Front));
	Renderer::binding_rasterizer_state = RS_State::Cull_Front;
	//デプスステンシルステート設定
	DxSystem::device_context->OMSetDepthStencilState(DxSystem::Get_DephtStencil_State(DS_State::LEqual), 1);
	Renderer::binding_depth_stencil_State = DS_State::LEqual;

	shared_ptr<Renderer> p_rend = nullptr;
	bool expired = false;
	bool disabled = false;
	for (weak_ptr<Renderer> r : Engine::render_manager->renderer_3D_list)
	{
		if (!r.expired())
		{
			p_rend = r.lock();
			if (p_rend->gameobject->Get_Active_In_Hierarchy())
			{
				if (p_rend->Get_Enabled())
				{
					p_rend->Render_Shadow(V, P);
				}
			}
			else
			{
				p_rend->is_disable = true;
				disabled = true;
			}
		}
		else
		{
			expired = true;
		}
	}
	if (expired)
	{
		auto removeIt = remove_if(Engine::render_manager->renderer_3D_list.begin(), Engine::render_manager->renderer_3D_list.end(), [](weak_ptr<Renderer> r) { return r.expired(); });
		Engine::render_manager->renderer_3D_list.erase(removeIt, Engine::render_manager->renderer_3D_list.end());
	}
	if (disabled)
	{
		auto removeIt = remove_if(Engine::render_manager->renderer_3D_list.begin(), Engine::render_manager->renderer_3D_list.end(), [](weak_ptr<Renderer> r) { shared_ptr<Renderer> rend = r.lock(); rend->is_called = false; return rend->is_disable; });
		Engine::render_manager->renderer_3D_list.erase(removeIt, Engine::render_manager->renderer_3D_list.end());
	}
}

void View_Texture::Render_3D(Matrix V, Matrix P)
{
	//トポロジー設定
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::shadow_manager->Set_PS_Resource();

	shared_ptr<Renderer> p_rend = nullptr;
	for (weak_ptr<Renderer> r : Engine::render_manager->renderer_3D_list)
	{
		p_rend = r.lock();
		if (p_rend->gameobject->Get_Active_In_Hierarchy())
		{
			if (p_rend->Get_Enabled())
			{
				p_rend->Render(V, P);
			}
		}
	}
}

void View_Texture::Render_2D(Matrix V, Matrix P)
{
	//トポロジー設定
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ブレンドステート設定
	DxSystem::device_context->OMSetBlendState(DxSystem::Get_Blend_State(BS_State::Alpha), nullptr, 0xFFFFFFFF);
	Renderer::binding_blend_state = BS_State::Alpha;
	//ラスタライザ―設定
	DxSystem::device_context->RSSetState(DxSystem::Get_Rasterizer_State(RS_State::Cull_None));
	Renderer::binding_rasterizer_state = RS_State::Cull_None;
	//デプスステンシルステート設定
	DxSystem::device_context->OMSetDepthStencilState(DxSystem::Get_DephtStencil_State(DS_State::None_No_Write), 1);
	Renderer::binding_depth_stencil_State = DS_State::None_No_Write;

	shared_ptr<Renderer> p_rend = nullptr;
	for (weak_ptr<Renderer> r : Engine::render_manager->renderer_2D_list)
	{
		p_rend = r.lock();
		if (p_rend->gameobject->Get_Active_In_Hierarchy())
		{
			if (p_rend->Get_Enabled())
			{
				p_rend->Render(V, P);
			}
		}
	}
}