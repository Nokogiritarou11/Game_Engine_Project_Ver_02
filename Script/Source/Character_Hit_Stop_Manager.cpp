#include "Character_Hit_Stop_Manager.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Character_Hit_Stop_Manager::Awake()
{
	//�����o�|�C���^�̎擾
	parameter = Get_Component<Character_Parameter>();
	animator = Get_Component<Animator>();
}

void Character_Hit_Stop_Manager::Update()
{
	//�q�b�g�X�g�b�v���L����
	if (is_stop)
	{
		//�^�C�}�[�̍X�V
		stop_timer -= Time::delta_time;
		//�^�C�}�[�̏I����
		if (stop_timer < 0.0f)
		{
			//�p�[�e�B�N�����ĊJ
			for (auto& particle : stop_particles)
			{
				particle.lock()->Play();
			}

			//�A�j���[�V�����̑��x��߂�
			animator.lock()->Set_Float("Hit_Stop_Speed", 1);
			parameter.lock()->pausing = false;

			//�����o�����Z�b�g
			stop_particles.clear();
			stop_timer = 0;
			is_stop = false;
		}
	}
}

void Character_Hit_Stop_Manager::Start_Hit_Stop(const float stop_time, const vector<weak_ptr<Particle>>& particles)
{
	//�����o�ւ̑��
	stop_timer = stop_time;
	stop_particles = particles;
	//�q�b�g�X�g�b�v�Ώۂ̃p�[�e�B�N�����ꎞ��~����
	for (auto& particle : stop_particles)
	{
		particle.lock()->Pause();
	}

	//�A�j���[�V�����̑��x��0�ɂ��邱�ƂŃq�b�g�X�g�b�v���s��
	animator.lock()->Set_Float("Hit_Stop_Speed", 0);
	parameter.lock()->pausing = true;

	is_stop = true;
}

void Character_Hit_Stop_Manager::Start_Hit_Stop(const float stop_time)
{
	//�����o�ւ̑��
	stop_timer = stop_time;
	//�q�b�g�X�g�b�v�Ώۂ̃p�[�e�B�N�����ꎞ��~����
	for (auto& particle : stop_particles)
	{
		particle.lock()->Pause();
	}

	//�A�j���[�V�����̑��x��0�ɂ��邱�ƂŃq�b�g�X�g�b�v���s��
	animator.lock()->Set_Float("Hit_Stop_Speed", 0);
	parameter.lock()->pausing = true;

	is_stop = true;
}

bool Character_Hit_Stop_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Character_Hit_Stop_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"�ݒ�ł���p�����[�^�͂���܂���");
	}
	return true;
}