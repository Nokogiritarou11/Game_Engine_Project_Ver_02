#include "Particle_Manager.h"
#include "Particle.h"
#include "Transform.h"
#include "Engine.h"
#include "View_Game.h"
#include "Time_Engine.h"
#include <math.h>

using namespace BeastEngine;
using namespace std;

Particle_Manager::Particle_Manager()
{
	// �G�t�F�N�g�̃����_���[�̍쐬
	renderer = EffekseerRendererDX11::Renderer::Create(DxSystem::device.Get(), DxSystem::device_context.Get(), 8000);

	// �G�t�F�N�g�̃}�l�[�W���[�̍쐬
	manager = Effekseer::Manager::Create(8000);
	manager->CreateCullingWorld(1000.0f, 1000.0f, 1000.0f, 1);

	// �`�惂�W���[���̐ݒ�
	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	manager->SetTrackRenderer(renderer->CreateTrackRenderer());
	manager->SetModelRenderer(renderer->CreateModelRenderer());

	// �e�N�X�`���A���f���A�}�e���A�����[�_�[�̐ݒ肷��B
	// ���[�U�[���Ǝ��Ŋg���ł���B���݂̓t�@�C������ǂݍ���ł���B
	manager->SetTextureLoader(renderer->CreateTextureLoader());
	manager->SetModelLoader(renderer->CreateModelLoader());
	manager->SetMaterialLoader(renderer->CreateMaterialLoader());
}

Particle_Manager::~Particle_Manager()
{
	if (renderer != nullptr)
	{
		renderer.Reset();
		renderer = nullptr;
	}
	if (manager != nullptr)
	{
		manager.Reset();
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
	particle_list.emplace_back(particle);
}

void Particle_Manager::Camera_Update(std::shared_ptr<Transform>& camera_trans, float FOV, float near_z, float far_z, float aspect)
{
	Vector3 pos = camera_trans->Get_Position();

	// ���_�ʒu���m��
	auto g_position = ::Effekseer::Vector3D(pos.x, pos.y, pos.z);

	// ���e�s���ݒ�
	renderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovRH(
		FOV, aspect, near_z, far_z));

	Vector3 focus = pos + camera_trans->Get_Forward();
	auto g_focus = ::Effekseer::Vector3D(focus.x, focus.y, focus.z);

	Vector3 up = camera_trans->Get_Up();
	auto g_up = ::Effekseer::Vector3D(up.x, up.y, up.z);

	// �J�����s���ݒ�
	renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH(g_position, g_focus, g_up));

}

void Particle_Manager::Update()
{
	shared_ptr<Particle> p_eff = nullptr;
	bool expired = false;
	for (auto& p : particle_list)
	{
		if (!p.expired())
		{
			p_eff = p.lock();
			if (p_eff->gameobject->Get_Active_In_Hierarchy())
			{
				if (Engine::particle_manager->manager->Exists(p_eff->handle))
				{
					Effekseer::Matrix43 mat;
					Matrix m = p_eff->transform->Get_World_Matrix();
					mat.Value[0][0] = m._11;mat.Value[0][1] = m._12;mat.Value[0][2] = m._13;
					mat.Value[1][0] = m._21;mat.Value[1][1] = m._22;mat.Value[1][2] = m._23;
					mat.Value[2][0] = m._31;mat.Value[2][1] = m._32;mat.Value[2][2] = m._33;
					mat.Value[3][0] = m._41;mat.Value[3][1] = m._42;mat.Value[3][2] = m._43;
					manager->SetBaseMatrix(p_eff->handle, mat);
					manager->SetSpeed(p_eff->handle, p_eff->play_speed);
				}
			}
			else
			{
				if (Engine::particle_manager->manager->Exists(p_eff->handle))
				{
					// �Đ����~����
					Engine::particle_manager->manager->StopEffect(p_eff->handle);
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
		auto removeIt = remove_if(particle_list.begin(), particle_list.end(), [](weak_ptr<Particle> p) { return p.expired(); });
		particle_list.erase(removeIt, particle_list.end());
	}

	manager->Update(60.0f / (1 / Time::delta_time));
}

void Particle_Manager::Render()
{
	// �G�t�F�N�g�̕`��J�n����
	renderer->BeginRendering();

	manager->CalcCulling(renderer->GetCameraProjectionMatrix(), false);
	// �G�t�F�N�g�̕`��
	manager->Draw();
	// �G�t�F�N�g�̕`��I������
	renderer->EndRendering();
}

void Particle_Manager::Reset()
{
	particle_list.clear();
	if (manager != nullptr)
	{
		manager.Reset();
		manager = nullptr;
		manager = Effekseer::Manager::Create(8000);

		manager->CreateCullingWorld(1000.0f, 1000.0f, 1000.0f, 1);
		// �`�惂�W���[���̐ݒ�
		manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
		manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
		manager->SetRingRenderer(renderer->CreateRingRenderer());
		manager->SetTrackRenderer(renderer->CreateTrackRenderer());
		manager->SetModelRenderer(renderer->CreateModelRenderer());

		// �e�N�X�`���A���f���A�}�e���A�����[�_�[�̐ݒ肷��B
		// ���[�U�[���Ǝ��Ŋg���ł���B���݂̓t�@�C������ǂݍ���ł���B
		manager->SetTextureLoader(renderer->CreateTextureLoader());
		manager->SetModelLoader(renderer->CreateModelLoader());
		manager->SetMaterialLoader(renderer->CreateMaterialLoader());
	}
}