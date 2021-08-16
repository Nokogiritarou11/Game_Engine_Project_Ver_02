#include <algorithm>
#include "Render_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Engine.h"
#include "Particle_Manager.h"
#include "Renderer.h"
#include "Material.h"
#include "Camera.h"
#include "SkinMesh_Renderer.h"
#include "Mesh_Renderer.h"
#include "Sprite_Renderer.h"
#include "Render_Texture.h"
#include "Light_Manager.h"
#include "Shadow_Manager.h"
#include "SkyBox.h"
#include "Light.h"
#include "Editor.h"
#include "Debug_Draw_Manager.h"
#include "Asset_Manager.h"
using namespace DirectX;
using namespace std;
using namespace BeastEngine;
using Microsoft::WRL::ComPtr;

Render_Manager::Render_Manager()
{
	scene_texture = make_unique<Render_Texture>(1920, 1080, true, DXGI_FORMAT_R16G16B16A16_UNORM);
	game_texture = make_unique<Render_Texture>(1920, 1080, true, DXGI_FORMAT_R16G16B16A16_UNORM);
	skybox = make_unique<SkyBox>();

	// 定数バッファの生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Constant_Buffer_Scene);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	HRESULT hr = DxSystem::device->CreateBuffer(&bd, nullptr, constant_buffer_scene.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	shadow_camera_transform = make_shared<Transform>();
	Engine::asset_manager->Registration_Asset(shadow_camera_transform);
	shadow_camera = make_shared<Camera>();
	Engine::asset_manager->Registration_Asset(shadow_camera);
	shadow_camera->transform = shadow_camera_transform;
	shadow_camera->is_orthographic = true;

	//デフォルトマテリアル
	sprite_material = Material::Create("Shader\\2D_Shader_VS.hlsl", "Shader\\2D_Shader_PS.hlsl");
}

void Render_Manager::Reset()
{
	for (auto& r : renderer_3D_list)
	{
		if (auto& rend = r.lock())
		{
			rend->enabled_old = false;
			rend->is_called = false;
		}
	}
	for (auto& r : renderer_2D_list)
	{
		if (auto& rend = r.lock())
		{
			rend->enabled_old = false;
			rend->is_called = false;
		}
	}
	renderer_3D_list.clear();
	renderer_2D_list.clear();
	camera_list.clear();
}

void Render_Manager::Add(const shared_ptr<Mesh_Renderer>& m_rend)
{
	renderer_3D_list.emplace_back(m_rend);
}
void Render_Manager::Add(const shared_ptr<SkinMesh_Renderer>& m_rend)
{
	renderer_3D_list.emplace_back(m_rend);
}
void Render_Manager::Add(const shared_ptr<Sprite_Renderer>& m_rend)
{
	renderer_2D_list.emplace_back(m_rend);
}
void Render_Manager::Add(const shared_ptr<Camera>& mono)
{
	camera_list.emplace_back(mono);
}

void Render_Manager::Check_Renderer()
{
	shared_ptr<Renderer> p_rend;
	//3Dオブジェクト
	{
		bool expired = false;
		for (auto& r : renderer_3D_list)
		{
			if (!r.expired())
			{
				p_rend = r.lock();
				p_rend->recalculated_frame = false;
			}
			else
			{
				expired = true;
			}
		}
		if (expired)
		{
			auto removeIt = remove_if(renderer_3D_list.begin(), renderer_3D_list.end(), [](auto& r) { return r.expired(); });
			renderer_3D_list.erase(removeIt, renderer_3D_list.end());
		}
	}
	//2Dオブジェクト
	{
		bool expired = false;
		for (auto& r : renderer_2D_list)
		{
			if (!r.expired())
			{
				p_rend = r.lock();
				p_rend->recalculated_frame = false;
			}
			else
			{
				expired = true;
			}
		}
		if (expired)
		{
			auto removeIt = remove_if(renderer_2D_list.begin(), renderer_2D_list.end(), [](weak_ptr<Renderer> r) { return r.expired(); });
			renderer_2D_list.erase(removeIt, renderer_2D_list.end());
		}
	}
}

void Render_Manager::Culling_Renderer(const Vector3& view_pos, const array<Vector4, 6>& planes)
{
	shared_ptr<Renderer> p_rend;
	for (auto& r : renderer_3D_list)
	{
		p_rend = r.lock();
		if (!p_rend->bounds.Get_Is_Culling_Frustum(p_rend->transform, planes))
		{
			Render_Obj obj = { r, p_rend->material[0]->render_queue, Vector3::DistanceSquared(p_rend->transform->Get_Position(), view_pos) };
			if (p_rend->material[0]->render_queue >= 3000)
			{
				alpha_list.emplace_back(obj);
			}
			else
			{
				opaque_list.emplace_back(obj);
			}
		}
	}
}

void Render_Manager::Sort_Renderer()
{
	sort(alpha_list.begin(), alpha_list.end(), [](const Render_Obj& a, const Render_Obj& b) {return (a.queue == b.queue) ? (a.z_distance > b.z_distance) : (a.queue < b.queue); });
	sort(opaque_list.begin(), opaque_list.end(), [](const Render_Obj& a, const Render_Obj& b) {return (a.queue == b.queue) ? (a.z_distance < b.z_distance) : (a.queue < b.queue); });
}

void Render_Manager::Render()
{
	Check_Renderer();
	Render_Game();
#if _DEBUG
	Render_Scene();
#endif
}

void Render_Manager::Render_Scene()
{
	Engine::editor->debug_camera->Update((float)scene_texture->screen_x, (float)scene_texture->screen_y);
	//シャドウマップ描画
	Render_Shadow(Engine::editor->debug_camera_transform);

	//通常描画
	{
		scene_texture->Set_Render_Target();
		// シーン用定数バッファ更新
		buffer_scene.view_projection_matrix = Engine::editor->debug_camera->view_projection_matrix;
		DxSystem::device_context->VSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
		DxSystem::device_context->PSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
		Update_Constant_Buffer();

		Render_Sky(Engine::editor->debug_camera_transform->Get_Position());
		Render_3D(Engine::editor->debug_camera);
		Engine::particle_manager->Render();

		Engine::debug_draw_manager->Render_Grid(Engine::editor->debug_camera_transform);
		Engine::debug_draw_manager->Render_Collider();
	}
}

void Render_Manager::Render_Game()
{
	shared_ptr<Camera> camera = nullptr;
	bool expired = false;
	for (auto& c : camera_list)
	{
		if (!c.expired())
		{
			camera = c.lock();
			if (camera->gameobject->Get_Active_In_Hierarchy())
			{
				if (camera->Get_Enabled())
				{
					camera->Update((float)game_texture->screen_x, (float)game_texture->screen_y);
					//シャドウマップ描画
					Render_Shadow(camera->transform);

					//通常描画
#if _DEBUG
					game_texture->Set_Render_Target();
#else
						// レンダーターゲットビュー設定
					DxSystem::Set_Default_View();
#endif
					// シーン用定数バッファ更新
					buffer_scene.view_projection_matrix = camera->view_projection_matrix;
					DxSystem::device_context->VSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
					DxSystem::device_context->PSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
					Update_Constant_Buffer();

					Render_Sky(camera->transform->Get_Position());
					Render_3D(camera);
					Engine::particle_manager->Render();
					Render_2D();
				}
			}
		}
		else
		{
			expired = true;
		}
	}
	if (expired)
	{
		auto removeIt = remove_if(camera_list.begin(), camera_list.end(), [](weak_ptr<Camera> m) { return m.expired(); });
		camera_list.erase(removeIt, camera_list.end());
	}
}

void Render_Manager::Render_3D(const shared_ptr<Camera>& camera)
{
	//トポロジー設定
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::shadow_manager->Set_PS_Resource();

	Culling_Renderer(camera->transform->Get_Position(), camera->frustum_planes);
	Sort_Renderer();

	shared_ptr<Renderer> p_rend = nullptr;
	for (auto& r : opaque_list)
	{
		p_rend = r.renderer.lock();
		if (p_rend->gameobject->Get_Active_In_Hierarchy())
		{
			if (p_rend->Get_Enabled())
			{
				p_rend->Render();
			}
		}
	}
	for (auto& r : alpha_list)
	{
		p_rend = r.renderer.lock();
		if (p_rend->gameobject->Get_Active_In_Hierarchy())
		{
			if (p_rend->Get_Enabled())
			{
				p_rend->Render();
			}
		}
	}
	opaque_list.clear();
	alpha_list.clear();
}

void Render_Manager::Render_2D()
{
	if (!renderer_2D_list.empty())
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

		sprite_material->Active();

		shared_ptr<Renderer> p_rend = nullptr;
		for (auto& r : renderer_2D_list)
		{
			p_rend = r.lock();
			if (p_rend->gameobject->Get_Active_In_Hierarchy())
			{
				if (p_rend->Get_Enabled())
				{
					p_rend->Render();
				}
			}
		}
	}
}

void Render_Manager::Render_Sky(const Vector3& pos)
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

void Render_Manager::Render_Shadow(const shared_ptr<Transform>& camera_transform)
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

void Render_Manager::Render_Shadow_Directional(const Vector3& color, const float& intensity, const shared_ptr<Transform>& light_transform, const shared_ptr<Transform>& camera_transform)
{
	shadow_camera->orthographic_size = Engine::shadow_manager->shadow_distance;

	const Vector3 Look_pos = camera_transform->Get_Position() + camera_transform->Get_Forward() * (Engine::shadow_manager->shadow_distance * 0.5f);
	shadow_camera->transform->Set_Position(Look_pos - light_transform->Get_Forward() * 800.0f);
	shadow_camera->transform->Set_Rotation(shadow_camera->transform->Look_At(Look_pos));
	shadow_camera->Update(0, 0);

	buffer_scene.view_projection_matrix = shadow_camera->view_projection_matrix;
	static const Matrix SHADOW_BIAS = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f };
	buffer_scene.shadow_matrix = shadow_camera->view_projection_matrix * SHADOW_BIAS;
	const Vector3 forward = light_transform->Get_Forward();
	buffer_scene.light_direction = { forward.x, forward.y, forward.z, 0 };
	buffer_scene.light_color = color;
	buffer_scene.bias = Engine::shadow_manager->shadow_bias;

	DxSystem::device_context->VSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
	Update_Constant_Buffer();

	//シャドウ用ステージング
	Engine::shadow_manager->Set_Shadow_Map_Texture();
	//トポロジー設定
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//ブレンドステート設定
	DxSystem::device_context->OMSetBlendState(DxSystem::Get_Blend_State(BS_State::Off), nullptr, 0xFFFFFFFF);
	Renderer::binding_blend_state = BS_State::Off;
	//ラスタライザ―設定
	DxSystem::device_context->RSSetState(DxSystem::Get_Rasterizer_State(RS_State::Cull_Back));
	Renderer::binding_rasterizer_state = RS_State::Cull_Back;
	//デプスステンシルステート設定
	DxSystem::device_context->OMSetDepthStencilState(DxSystem::Get_DephtStencil_State(DS_State::LEqual), 1);
	Renderer::binding_depth_stencil_State = DS_State::LEqual;

	Culling_Renderer(shadow_camera->transform->Get_Position(), shadow_camera->frustum_planes);
	Sort_Renderer();

	shared_ptr<Renderer> p_rend = nullptr;
	for (auto& r : opaque_list)
	{
		p_rend = r.renderer.lock();
		if (p_rend->gameobject->Get_Active_In_Hierarchy())
		{
			if (p_rend->Get_Enabled())
			{
				p_rend->Render_Shadow();
			}
		}
	}
	opaque_list.clear();

	//ラスタライザ―設定
	DxSystem::device_context->RSSetState(DxSystem::Get_Rasterizer_State(RS_State::Cull_None));
	Renderer::binding_rasterizer_state = RS_State::Cull_None;
	Engine::shadow_manager->Filtering_Gaussian();
}

void Render_Manager::Update_Constant_Buffer()
{
	UINT subresourceIndex = 0;
	D3D11_MAPPED_SUBRESOURCE mapped;
	auto hr = DxSystem::device_context->Map(constant_buffer_scene.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, &buffer_scene, sizeof(Constant_Buffer_Scene));
		DxSystem::device_context->Unmap(constant_buffer_scene.Get(), subresourceIndex);
	}
}