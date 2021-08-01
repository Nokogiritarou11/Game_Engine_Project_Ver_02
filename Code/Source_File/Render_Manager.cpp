#include "Render_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Engine.h"
#include "Particle_Manager.h"
#include "Renderer.h"
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
using namespace DirectX;
using namespace std;
using namespace BeastEngine;
using Microsoft::WRL::ComPtr;

Render_Manager::Render_Manager()
{
	scene_texture = make_unique<Render_Texture>();
	game_texture = make_unique<Render_Texture>();
	skybox = make_unique<SkyBox>();

	// 定数バッファの生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Constant_Buffer_Scene);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	HRESULT hr = DxSystem::device->CreateBuffer(&bd, nullptr, constant_buffer_scene.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
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

void Render_Manager::Add(shared_ptr<Mesh_Renderer> m_rend)
{
	renderer_3D_list.emplace_back(m_rend);
}
void Render_Manager::Add(shared_ptr<SkinMesh_Renderer> m_rend)
{
	renderer_3D_list.emplace_back(m_rend);
}
void Render_Manager::Add(shared_ptr<Sprite_Renderer> m_rend)
{
	renderer_2D_list.emplace_back(m_rend);
}
void Render_Manager::Add(shared_ptr<Camera> mono)
{
	camera_list.emplace_back(mono);
}

void Render_Manager::Check_Renderer()
{
	shared_ptr<Renderer> p_rend = nullptr;
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
			auto removeIt = remove_if(renderer_3D_list.begin(), renderer_3D_list.end(), [](weak_ptr<Renderer> r) { return r.expired(); });
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

void Render_Manager::Render()
{
	Check_Renderer();
#if _DEBUG
	Render_Scene();
#endif
	Render_Game();
}

void Render_Manager::Render_Scene()
{
	Engine::particle_manager->Camera_Update(Engine::editor->debug_camera_transform, Engine::editor->fov_y, Engine::editor->near_z, Engine::editor->far_z, Engine::editor->aspect);
	//シャドウマップ描画
	Render_Shadow(Engine::editor->debug_camera_transform);

	//通常描画
	{
		scene_texture->Set_Render_Target();
		// シーン用定数バッファ更新
		buffer_scene.view_projection_matrix = Engine::editor->debug_camera_view_matrix * Engine::editor->debug_camera_projection_matrix;
		DxSystem::device_context->VSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
		DxSystem::device_context->PSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
		DxSystem::device_context->UpdateSubresource(constant_buffer_scene.Get(), 0, 0, &buffer_scene, 0, 0);

		Render_Sky(Engine::editor->debug_camera_transform->Get_Position());
		Render_3D();
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
					// プロジェクション行列を作成
					float fov_y = XMConvertToRadians(camera->fov);	// 画角
					float aspect = (float)game_texture->screen_x / (float)game_texture->screen_y;	// 画面比率

					camera->projection_matrix = XMMatrixPerspectiveFovRH(fov_y, aspect, camera->near_z, camera->far_z);

					// ビュー行列を作成
					//{
					Vector3 eye_v = camera->transform->Get_Position();
					Vector3 focus_v = eye_v + camera->transform->Get_Forward();

					Vector3 camForward = XMVector3Normalize(focus_v - eye_v);    // Get forward vector based on target
					camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
					camForward = XMVector3Normalize(camForward);
					Vector3 camRight = { -XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward) };

					XMVECTOR up_v = camera->transform->Get_Up();
					camera->view_matrix = XMMatrixLookAtRH(eye_v, focus_v, up_v);
					//}

					Engine::particle_manager->Camera_Update(camera->transform, fov_y, camera->near_z, camera->far_z, aspect);
					//シャドウマップ描画
					Render_Shadow(camera->transform);

					//通常描画
					{
#if _DEBUG
						game_texture->Set_Render_Target();
#else
						// レンダーターゲットビュー設定
						DxSystem::Set_Default_View();
#endif
						// シーン用定数バッファ更新
						buffer_scene.view_projection_matrix = camera->view_matrix * camera->projection_matrix;
						DxSystem::device_context->VSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
						DxSystem::device_context->PSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
						DxSystem::device_context->UpdateSubresource(constant_buffer_scene.Get(), 0, 0, &buffer_scene, 0, 0);

						Render_Sky(camera->transform->Get_Position());
						Render_3D();
						Engine::particle_manager->Render();
						Render_2D();
					}
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

void Render_Manager::Render_3D()
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
				p_rend->Render();
			}
		}
	}
}

void Render_Manager::Render_2D()
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
				p_rend->Render();
			}
		}
	}
}

void Render_Manager::Render_Sky(Vector3& pos)
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

void Render_Manager::Render_Shadow(std::shared_ptr<Transform>& camera_transform)
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

void Render_Manager::Render_Shadow_Directional(Vector4 color, float intensity, std::shared_ptr<Transform>& light_transform, std::shared_ptr<Transform>& camera_transform)
{
	Matrix V, P, VP;
	const float size = Engine::shadow_manager->shadow_distance;
	const Vector3 Look_pos = camera_transform->Get_Position() + camera_transform->Get_Forward() * (size * 0.5f);
	V = XMMatrixLookAtRH(Look_pos - light_transform->Get_Forward() * 800.0f, Look_pos, light_transform->Get_Up());
	//V = XMMatrixLookAtRH(-light_transform->Get_Forward() * 900.0f, Vector3(0,0,0), light_transform->Get_Up());
	P = XMMatrixOrthographicRH(size, size, 100.0f, 1000.0f);
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
	buffer_scene.bias = Engine::shadow_manager->shadow_bias;

	DxSystem::device_context->VSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
	DxSystem::device_context->UpdateSubresource(constant_buffer_scene.Get(), 0, 0, &buffer_scene, 0, 0);

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
	DxSystem::device_context->OMSetDepthStencilState(DxSystem::Get_DephtStencil_State(DS_State::Less), 1);
	Renderer::binding_depth_stencil_State = DS_State::Less;

	shared_ptr<Renderer> p_rend = nullptr;
	for (weak_ptr<Renderer> r : Engine::render_manager->renderer_3D_list)
	{
		p_rend = r.lock();
		if (p_rend->gameobject->Get_Active_In_Hierarchy())
		{
			if (p_rend->Get_Enabled())
			{
				p_rend->Render_Shadow();
			}
		}
	}
}