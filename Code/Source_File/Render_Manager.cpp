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
#include "Misc.h"
using namespace DirectX;
using namespace std;
using namespace BeastEngine;
using Microsoft::WRL::ComPtr;

Render_Manager::Render_Manager()
{
	scene_texture = make_shared<Render_Texture>(1920, 1080, true, DXGI_FORMAT_R8G8B8A8_UNORM);
	game_texture = make_shared<Render_Texture>(1920, 1080, true, DXGI_FORMAT_R8G8B8A8_UNORM);
	skybox = make_unique<SkyBox>();

	{
		// 定数バッファの生成
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(Constant_Buffer_Scene);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		const HRESULT hr = DxSystem::device->CreateBuffer(&bd, nullptr, constant_buffer_scene.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		//	頂点バッファ作成
		constexpr Vertex v[] = {
			Vector3(-1.0f, 1.0f,0), Vector2(0,0), Vector4(1,1,1,1), //左上
			Vector3(1.0f, 1.0f,0),  Vector2(1,0), Vector4(1,1,1,1), //右上
			Vector3(-1.0f,-1.0f,0), Vector2(0,1), Vector4(1,1,1,1), //左下
			Vector3(1.0f,-1.0f,0),  Vector2(1,1), Vector4(1,1,1,1), //右下
		};
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(v);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA res;
		ZeroMemory(&res, sizeof(res));
		res.pSysMem = v;

		DxSystem::device->CreateBuffer(&bd, &res, main_texture_vertex_buffer.GetAddressOf());

		//	サンプラステート作成
		D3D11_SAMPLER_DESC sd = {};
		sd.Filter = D3D11_FILTER_ANISOTROPIC;	  // 異方性フィルタ
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // U
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // V
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // W
		sd.MipLODBias = 0;
		sd.MaxAnisotropy = 4; // 最大異方性(1Pixelあたりのテクスチャ点数)
		sd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT hr = DxSystem::device->CreateSamplerState(&sd, sampler.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//シャドー用のカメラ生成
	shadow_camera_transform = make_shared<Transform>();
	Engine::asset_manager->Registration_Asset(shadow_camera_transform);
	shadow_camera = make_shared<Camera>();
	Engine::asset_manager->Registration_Asset(shadow_camera);
	shadow_camera->transform = shadow_camera_transform;
	shadow_camera->is_orthographic = true;
	shadow_camera->far_z = 100.0f;
	shadow_camera->near_z = 10;

	//スプライト用のデフォルトマテリアル作成
	sprite_material = Material::Create("Shader\\2D_Shader_VS.hlsl", "Shader\\2D_Shader_PS.hlsl");
	sprite_material->Set_Blend_State(BS_State::Alpha);
	sprite_material->Set_Rasterizer_State(RS_State::Cull_None);
	sprite_material->Set_Depth_Stencil_State(DS_State::None_No_Write);
}

void Render_Manager::Reset()
{
	for (auto& r : renderer_3D_list)
	{
		if (const auto& rend = r.lock())
		{
			rend->enabled_old = false;
			rend->is_called = false;
		}
	}
	for (auto& r : renderer_2D_list)
	{
		if (const auto& rend = r.lock())
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
	//3Dオブジェクトの生存確認
	{
		bool expired = false;
		for (auto& r : renderer_3D_list)
		{
			if (const auto& rend = r.lock())
			{
				rend->Recalculate_Frame();
			}
			else
			{
				expired = true;
			}
		}
		if (expired)
		{
			const auto removeIt = remove_if(renderer_3D_list.begin(), renderer_3D_list.end(), [](auto& r) { return r.expired(); });
			renderer_3D_list.erase(removeIt, renderer_3D_list.end());
		}
	}
	//2Dオブジェクトの生存確認
	{
		bool expired = false;
		for (auto& r : renderer_2D_list)
		{
			if (const auto& rend = r.lock())
			{
				rend->Recalculate_Frame();
			}
			else
			{
				expired = true;
			}
		}
		if (expired)
		{
			const auto remove_it = remove_if(renderer_2D_list.begin(), renderer_2D_list.end(), [](weak_ptr<Renderer> r) { return r.expired(); });
			renderer_2D_list.erase(remove_it, renderer_2D_list.end());
		}
	}
}

void Render_Manager::Culling_Renderer(const Vector3& view_pos, const array<Vector4, 6>& planes)
{
	for (auto& r : renderer_3D_list)
	{
		if (const auto& p_rend = r.lock(); p_rend->can_render)
		{
			//アクティブなもののみ
			if (p_rend->gameobject->Get_Active_In_Hierarchy())
			{
				if (p_rend->Get_Enabled())
				{
					//視錐台カリング
					if (!p_rend->bounds.Get_Is_Culling_Frustum(p_rend->transform, planes))
					{
						for (int i = 0; i < p_rend->subset_count; ++i)
						{
							Render_Obj obj = { r, i, p_rend->material[p_rend->subset_material_index[i]]->render_queue, Vector3::DistanceSquared(p_rend->transform->Get_Position(), view_pos) };
							//半透明なものは後回しに
							if (p_rend->material[p_rend->subset_material_index[i]]->render_queue >= 2500)
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
			}
		}
	}
}

void Render_Manager::Sort_Renderer()
{
	//カメラからの距離とレンダーキューでソートする
	sort(alpha_list.begin(), alpha_list.end(), [](const Render_Obj& a, const Render_Obj& b) {return (a.queue == b.queue) ? (a.z_distance > b.z_distance) : (a.queue < b.queue); });
	sort(opaque_list.begin(), opaque_list.end(), [](const Render_Obj& a, const Render_Obj& b) {return (a.queue == b.queue) ? (a.z_distance < b.z_distance) : (a.queue < b.queue); });
}

void Render_Manager::Render()
{
	Check_Renderer();

	if (render_game)
	{
		Render_Game();
	}

#if _DEBUG
	if (render_scene)
	{
		Render_Scene();
	}
#else
	Render_Main();
#endif
}

void Render_Manager::Render_Scene()
{
	Engine::editor->debug_camera->Update(static_cast<float>(scene_texture->screen_x), static_cast<float>(scene_texture->screen_y));
	//シャドウマップ描画
	Render_Shadow(Engine::editor->debug_camera_transform);

	//通常描画
	{
		scene_texture->Set_Render_Target();
		staging_texture = scene_texture;
		// シーン用定数バッファ更新
		DxSystem::device_context->VSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
		DxSystem::device_context->PSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());

		const Vector3& pos = Engine::editor->debug_camera_transform->Get_Position();
		buffer_scene.camera_position = Vector4(pos.x, pos.y, pos.z, 0);
		const Vector3& dir = Engine::editor->debug_camera_transform->Get_Forward();
		buffer_scene.camera_direction = Vector4(dir.x, dir.y, dir.z, 0);
		buffer_scene.view_projection_matrix = Engine::editor->debug_camera->view_projection_matrix;
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
	bool expired = false;
	for (auto& c : camera_list)
	{
		if (const auto& camera = c.lock())
		{
			if (camera->gameobject->Get_Active_In_Hierarchy())
			{
				if (camera->Get_Enabled())
				{
					camera->Update(static_cast<float>(game_texture->screen_x), static_cast<float>(game_texture->screen_y));
					//シャドウマップ描画
					Render_Shadow(camera->transform);

					//通常描画
					game_texture->Set_Render_Target();
					staging_texture = game_texture;
					// シーン用定数バッファ更新
					DxSystem::device_context->VSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
					DxSystem::device_context->PSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());

					const Vector3& pos = camera->transform->Get_Position();
					buffer_scene.camera_position = Vector4(pos.x, pos.y, pos.z, 0);
					const Vector3& dir = camera->transform->Get_Forward();
					buffer_scene.camera_direction = Vector4(dir.x, dir.y, dir.z, 0);
					buffer_scene.view_projection_matrix = camera->view_projection_matrix;
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
		const auto remove_it = remove_if(camera_list.begin(), camera_list.end(), [](weak_ptr<Camera> m) { return m.expired(); });
		camera_list.erase(remove_it, camera_list.end());
	}
}

void Render_Manager::Render_Main()
{
	DxSystem::Set_Default_View();
	//トポロジー設定
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	sprite_material->Activate();

	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;
	DxSystem::device_context->IASetVertexBuffers(0, 1, main_texture_vertex_buffer.GetAddressOf(), &stride, &offset);
	//テクスチャの設定
	DxSystem::device_context->PSSetShaderResources(1, 1, game_texture->Get_Texture().GetAddressOf());
	DxSystem::device_context->PSSetSamplers(1, 1, sampler.GetAddressOf());

	// 描画
	DxSystem::device_context->Draw(4, 0);
}

void Render_Manager::Render_3D(const shared_ptr<Camera>& camera)
{
	//トポロジー設定
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::shadow_manager->Set_PS_Resource();

	Culling_Renderer(camera->transform->Get_Position(), camera->frustum_planes);
	Sort_Renderer();

	for (auto& r : opaque_list)
	{
		r.renderer.lock()->Render(r.subset_number);
	}

	for (auto& r : alpha_list)
	{
		r.renderer.lock()->Render();
	}

	opaque_list.clear();
	alpha_list.clear();
}

void Render_Manager::Render_2D() const
{
	if (!renderer_2D_list.empty())
	{
		//トポロジー設定
		DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		sprite_material->Activate();

		for (auto& r : renderer_2D_list)
		{
			const auto& p_rend = r.lock();
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

void Render_Manager::Render_Sky(const Vector3& pos) const
{
	//トポロジー設定
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	skybox->Render(pos);
}

void Render_Manager::Render_Shadow(const shared_ptr<Transform>& camera_transform)
{
	for (auto itr = Engine::light_manager->light_list.begin(); itr != Engine::light_manager->light_list.end();)
	{
		if (const auto& m_light = itr->lock())
		{
			if (m_light->gameobject->Get_Active_In_Hierarchy())
			{
				if (m_light->Get_Enabled())
				{
					Render_Shadow_Directional(m_light->color, m_light->intensity, m_light->transform, camera_transform);
				}
				++itr;
			}
		}
		else
		{
			itr = Engine::light_manager->light_list.erase(itr);
		}
	}
}

void Render_Manager::Render_Shadow_Directional(const Vector3& color, const float& intensity, const shared_ptr<Transform>& light_transform, const shared_ptr<Transform>& camera_transform)
{
	//シャドウ描画用カメラ姿勢設定
	shadow_camera->orthographic_size = Engine::shadow_manager->shadow_distance;
	const Vector3 look_pos = camera_transform->Get_Position() + camera_transform->Get_Forward() * (Engine::shadow_manager->shadow_distance * 0.5f); shadow_camera->transform->Set_Position(look_pos - light_transform->Get_Forward() * 50.0f);
	shadow_camera->transform->Set_Rotation(shadow_camera->transform->Look_At(look_pos));
	shadow_camera->Update(0, 0);

	buffer_scene.view_projection_matrix = shadow_camera->view_projection_matrix;
	//VSMの行列
	static constexpr Matrix SHADOW_BIAS = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f };
	buffer_scene.shadow_matrix = shadow_camera->view_projection_matrix * SHADOW_BIAS;

	//コンスタントバッファ更新
	const Vector3& pos = shadow_camera->transform->Get_Position();
	buffer_scene.camera_position = Vector4(pos.x, pos.y, pos.z, 0);
	const Vector3& dir = shadow_camera->transform->Get_Forward();
	buffer_scene.camera_direction = Vector4(dir.x, dir.y, dir.z, 0);

	const Vector3& forward = light_transform->Get_Forward();
	buffer_scene.light_direction = Vector4(forward.x, forward.y, forward.z, 0);
	buffer_scene.light_color = color;
	buffer_scene.bias = Engine::shadow_manager->shadow_bias;

	DxSystem::device_context->VSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
	Update_Constant_Buffer();

	//シャドウ用ステージング
	Engine::shadow_manager->Set_Shadow_Map_Texture();
	//トポロジー設定
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Culling_Renderer(shadow_camera->transform->Get_Position(), shadow_camera->frustum_planes);
	Sort_Renderer();

	//描画実行
	for (auto& r : opaque_list)
	{
		const auto& p_rend = r.renderer.lock();
		if (p_rend->gameobject->Get_Active_In_Hierarchy())
		{
			if (p_rend->Get_Enabled())
			{
				p_rend->Render_Shadow(r.subset_number);
			}
		}
	}
	opaque_list.clear();
	//VSMぼかし実行
	Engine::shadow_manager->Filtering_Gaussian();
}

void Render_Manager::Update_Constant_Buffer() const
{
	constexpr UINT subresource_index = 0;
	D3D11_MAPPED_SUBRESOURCE mapped;
	const auto hr = DxSystem::device_context->Map(constant_buffer_scene.Get(), subresource_index, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, &buffer_scene, sizeof(Constant_Buffer_Scene));
		DxSystem::device_context->Unmap(constant_buffer_scene.Get(), subresource_index);
	}
}
