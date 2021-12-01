#include "Time_Manager.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Time_Manager::Update()
{
	//�X���[���ʍĐ�����
	if (is_slow)
	{
		if (active_slow)
		{
			//�X���[���ʒ��Ȃ̂�delta_time�ɋt�����|���Ď��ۂ̎��Ԃ��Z�o����
			slow_timer -= Time::delta_time * inverse_time_scale;
			if (slow_timer < 0.0f)
			{
				//�X���[���ʂ̏I��
				Time::time_scale = 1;
				is_slow = false;
				active_slow = false;
			}
		}
		else
		{
			//���ʔ����f�B���C��
			//�f�B���C���Ԃ��o�߂���܂ŏ�������������
			delay_timer -= Time::delta_time;
			if (delay_timer <= 0)
			{
				Time::time_scale = slow_scale;
				active_slow = true;
			}
		}
	}
}

void Time_Manager::Start_Time_Slow(const float delay_time, const float slow_time, const float slow_speed)
{
	//�d���h�~
	if (!is_slow)
	{
		//�X���[���ʂ̃p�����[�^��ݒ肷��
		slow_timer = slow_time;
		delay_timer = delay_time;
		inverse_time_scale = 1 / slow_speed;
		slow_scale = slow_speed;
		is_slow = true;
		active_slow = false;
	}
}

bool Time_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Time_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"�ݒ�ł���p�����[�^�͂���܂���");
	}
	return true;
}