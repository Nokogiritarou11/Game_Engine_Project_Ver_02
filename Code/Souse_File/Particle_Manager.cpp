#include "Particle_Manager.h"
#include "Particle.h"
#include "Transform.h"
#include "Engine.h"
#include "View_Game.h"
#include <math.h>

using namespace std;

Particle_Manager::Particle_Manager()
{
	// エフェクトのレンダラーの作成
	renderer = EffekseerRendererDX11::Renderer::Create(DxSystem::Device.Get(), DxSystem::DeviceContext.Get(), 8000);

	// エフェクトのマネージャーの作成
	manager = Effekseer::Manager::Create(8000);
	manager->CreateCullingWorld(1000.0f, 1000.0f, 1000.0f, 1);

	// 描画モジュールの設定
	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	manager->SetTrackRenderer(renderer->CreateTrackRenderer());
	manager->SetModelRenderer(renderer->CreateModelRenderer());

	// テクスチャ、モデル、マテリアルローダーの設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	manager->SetTextureLoader(renderer->CreateTextureLoader());
	manager->SetModelLoader(renderer->CreateModelLoader());
	manager->SetMaterialLoader(renderer->CreateMaterialLoader());
}

Particle_Manager::~Particle_Manager()
{
	if (renderer != nullptr)
	{
		renderer->Destroy();
		renderer = nullptr;
	}
	if (manager != nullptr)
	{
		manager->Destroy();
		manager = nullptr;
	}
	for (auto pair : effect_cache)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
}

void Particle_Manager::Add(weak_ptr<Particle> particle)
{
	Particle_List.emplace_back(particle);
}

void Particle_Manager::Camera_Update(std::shared_ptr<Transform>& camera_trans, float FOV, float near_z, float far_z, float aspect)
{
	Vector3 pos = camera_trans->Get_position();

	// 視点位置を確定
	auto g_position = ::Effekseer::Vector3D(pos.x, pos.y, pos.z);

	// 投影行列を設定
	renderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovLH(
		FOV, aspect, near_z, far_z));

	Vector3 focus = pos + camera_trans->Get_forward();
	auto g_focus = ::Effekseer::Vector3D(focus.x, focus.y, focus.z);

	Vector3 up = camera_trans->Get_up();
	auto g_up = ::Effekseer::Vector3D(up.x, up.y, up.z);

	// カメラ行列を設定
	renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtLH(g_position, g_focus, g_up));

}

void Particle_Manager::Update()
{
	shared_ptr<Particle> p_eff = nullptr;
	bool expired = false;
	bool disabled = false;
	for (weak_ptr<Particle> r : Particle_List)
	{
		if (!r.expired())
		{
			p_eff = r.lock();
			if (p_eff->gameObject->activeInHierarchy())
			{
				if (Engine::particle_manager->manager->Exists(p_eff->handle))
				{
					Effekseer::Matrix43 mat;
					Matrix m = p_eff->transform->Get_world_matrix();
					mat.Value[0][0] = m._11;mat.Value[0][1] = m._12;mat.Value[0][2] = m._13;
					mat.Value[1][0] = m._21;mat.Value[1][1] = m._22;mat.Value[1][2] = m._23;
					mat.Value[2][0] = m._31;mat.Value[2][1] = m._32;mat.Value[2][2] = m._33;
					mat.Value[3][0] = m._41;mat.Value[3][1] = m._42;mat.Value[3][2] = m._43;
					manager->SetBaseMatrix(p_eff->handle, mat);
					manager->SetSpeed(p_eff->handle, p_eff->Play_Speed);
				}
			}
			else
			{
				if (Engine::particle_manager->manager->Exists(p_eff->handle))
				{
					// 再生を停止する
					Engine::particle_manager->manager->StopEffect(p_eff->handle);
				}
				p_eff->Disable_flg = true;
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
		auto removeIt = remove_if(Particle_List.begin(), Particle_List.end(), [](weak_ptr<Particle> r) { return r.expired(); });
		Particle_List.erase(removeIt, Particle_List.end());
	}
	if (disabled)
	{
		auto removeIt = remove_if(Particle_List.begin(), Particle_List.end(), [](weak_ptr<Particle> r) { shared_ptr<Particle> p = r.lock(); p->IsCalled = false; return p->Disable_flg; });
		Particle_List.erase(removeIt, Particle_List.end());
	}

	manager->Update(60.0f / (1 / Time::deltaTime));
}

void Particle_Manager::Render()
{
	// エフェクトの描画開始処理
	renderer->BeginRendering();

	manager->CalcCulling(renderer->GetCameraProjectionMatrix(), false);
	// エフェクトの描画
	manager->Draw();
	// エフェクトの描画終了処理
	renderer->EndRendering();
}

void Particle_Manager::Reset()
{
	Particle_List.clear();
	if (manager != nullptr)
	{
		manager->Destroy();
		manager = nullptr;
		manager = Effekseer::Manager::Create(8000);

		manager->CreateCullingWorld(1000.0f, 1000.0f, 1000.0f, 1);
		// 描画モジュールの設定
		manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
		manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
		manager->SetRingRenderer(renderer->CreateRingRenderer());
		manager->SetTrackRenderer(renderer->CreateTrackRenderer());
		manager->SetModelRenderer(renderer->CreateModelRenderer());

		// テクスチャ、モデル、マテリアルローダーの設定する。
		// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
		manager->SetTextureLoader(renderer->CreateTextureLoader());
		manager->SetModelLoader(renderer->CreateModelLoader());
		manager->SetMaterialLoader(renderer->CreateMaterialLoader());
	}
}