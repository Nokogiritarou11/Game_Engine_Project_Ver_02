#include "Particle_Manager.h"
#include "Particle.h"
#include "Transform.h"
#include "Engine.h"
#include "Render_Manager.h"
#include "Render_Texture.h"
#include "Scene_Manager.h"
#include "Time_Engine.h"

using namespace BeastEngine;
using namespace std;

Particle_Manager::Particle_Manager()
{
	// �G�t�F�N�g�̃����_���[�̍쐬
	renderer = EffekseerRendererDX11::Renderer::Create(DxSystem::device.Get(), DxSystem::device_context.Get(), 8000, D3D11_COMPARISON_GREATER_EQUAL, true);

	//�c�ݕ`��p�̃R�[���o�b�N��ݒ�
	distorting_callback = new Distorting_Callback;
	renderer->SetDistortingCallback(distorting_callback);

	//���̃G���W���̓��j�A�X�y�[�X�Ȃ̂�Effekseer���ŃK���}�J���[���Č�������
	renderer->SetMaintainGammaColorInLinearColorSpace(true);

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

	//InverseZ�p�t�s��쐬
	reverse.Values[0][0] = 1.0f;
	reverse.Values[0][1] = 0.0f;
	reverse.Values[0][2] = 0.0f;
	reverse.Values[0][3] = 0.0f;
	reverse.Values[1][0] = 0.0f;
	reverse.Values[1][1] = 1.0f;
	reverse.Values[1][2] = 0.0f;
	reverse.Values[1][3] = 0.0f;
	reverse.Values[2][0] = 0.0f;
	reverse.Values[2][1] = 0.0f;
	reverse.Values[2][2] = -1.0f;
	reverse.Values[2][3] = 0.0f;
	reverse.Values[3][0] = 0.0f;
	reverse.Values[3][1] = 0.0f;
	reverse.Values[3][2] = 1.0f;
	reverse.Values[3][3] = 1.0f;
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
}

void Particle_Manager::Add(weak_ptr<Particle> particle)
{
	particle_list.emplace_back(particle);
}

void Particle_Manager::Camera_Update(const std::shared_ptr<Transform>& camera_trans, const float fov, const float near_z, const float far_z, const float aspect) const
{
	const Vector3 pos = camera_trans->Get_Position();

	// ���_�ʒu���m��
	const auto g_position = Effekseer::Vector3D(pos.x, pos.y, pos.z);

	// ���e�s���ݒ�
	Effekseer::Matrix44 perspective;
	Effekseer::Matrix44::Mul(perspective, Effekseer::Matrix44().PerspectiveFovRH(fov, aspect, near_z, far_z), reverse);
	renderer->SetProjectionMatrix(perspective);

	const Vector3 focus = pos + camera_trans->Get_Forward();
	const auto g_focus = Effekseer::Vector3D(focus.x, focus.y, focus.z);

	const Vector3 up = camera_trans->Get_Up();
	const auto g_up = Effekseer::Vector3D(up.x, up.y, up.z);

	// �J�����s���ݒ�
	renderer->SetCameraMatrix(
		Effekseer::Matrix44().LookAtRH(g_position, g_focus, g_up));

}

void Particle_Manager::Update()
{
	bool expired = false;
	for (auto& p : particle_list)
	{
		if (const auto& p_eff = p.lock())
		{
			if (p_eff->gameobject->Get_Active_In_Hierarchy())
			{
				if (manager->Exists(p_eff->handle))
				{
					Effekseer::Matrix43 mat;
					const Matrix m = p_eff->transform->Get_World_Matrix();
					mat.Value[0][0] = m._11; mat.Value[0][1] = m._12; mat.Value[0][2] = m._13;
					mat.Value[1][0] = m._21; mat.Value[1][1] = m._22; mat.Value[1][2] = m._23;
					mat.Value[2][0] = m._31; mat.Value[2][1] = m._32; mat.Value[2][2] = m._33;
					mat.Value[3][0] = m._41; mat.Value[3][1] = m._42; mat.Value[3][2] = m._43;
					manager->SetBaseMatrix(p_eff->handle, mat);
					manager->SetSpeed(p_eff->handle, p_eff->play_speed);
				}
				else
				{
					p_eff->is_playing = false;
				}
			}
			else
			{
				//�C���X�^���X������ł����
				if (manager->Exists(p_eff->handle))
				{
					// �Đ����~����
					manager->StopEffect(p_eff->handle);
					p_eff->is_playing = false;
				}
			}
		}
		else
		{
			expired = true;
		}
	}
	//�폜����Ă����ꍇ���X�g������폜
	if (expired)
	{
		const auto& remove_it = remove_if(particle_list.begin(), particle_list.end(), [](weak_ptr<Particle> p) { return p.expired(); });
		particle_list.erase(remove_it, particle_list.end());
	}

	if (!Engine::scene_manager->pause)
	{
		//���Ԃ�i�߂�
		manager->Update(60.0f / (1 / Time::delta_time));
	}
}

void Particle_Manager::Render() const
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

bool Distorting_Callback::OnDistorting(EffekseerRenderer::Renderer* renderer)
{
	Engine::render_manager->staging_texture->Update_Copy_Texture();
	renderer->SetBackground(Engine::render_manager->staging_texture->Get_Back_Ground_Texture());
	return true;
}
