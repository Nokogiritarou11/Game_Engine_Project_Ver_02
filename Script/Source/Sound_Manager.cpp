#include "Sound_Manager.h"
using namespace std;
using namespace BeastEngine;

void Sound_Manager::Awake()
{
	bgm_list.emplace_back(transform->Get_Child(0).lock()->Get_Component<AudioSource>());
	bgm_list.emplace_back(transform->Get_Child(1).lock()->Get_Component<AudioSource>());
	bgm_list.emplace_back(transform->Get_Child(2).lock()->Get_Component<AudioSource>());
}

void Sound_Manager::Update()
{
	if (fade_state == Fade_State::Stop)
	{
		//�\�񂳂ꂽ�Ȃ�����΃t�F�[�h�C������Đ��J�n
		if (next_bgm != BGM_Name::None)
		{
			playing_source = bgm_list[static_cast<int>(next_bgm) - 1];
			playing_source.lock()->Play();
			playing_source.lock()->Set_Volume(0);
			fade_state = Fade_State::Fade_In;
			next_bgm = BGM_Name::None;
		}
	}
	else if (fade_state != Fade_State::Playing)
	{
		fade_timer += Time::delta_time;
		if (fade_state == Fade_State::Fade_In)
		{
			//�t�F�[�h�C��
			if (const float rate = fade_timer / fade_time; rate < 1)
			{
				playing_source.lock()->Set_Volume(Mathf::Lerp(0, bgm_volume, rate));
			}
			else
			{
				playing_source.lock()->Set_Volume(bgm_volume);
				fade_state = Fade_State::Playing;
			}
		}
		else if (fade_state == Fade_State::Fade_Out)
		{
			//�t�F�[�h�A�E�g
			if (const float rate = fade_timer / fade_time; rate < 1)
			{
				playing_source.lock()->Set_Volume(Mathf::Lerp(bgm_volume, 0, rate));
			}
			else
			{
				playing_source.lock()->Set_Volume(0);
				playing_source.lock()->Stop();
				fade_state = Fade_State::Stop;
			}
		}
	}
}

void Sound_Manager::Play_BGM(const BGM_Name name)
{
	//���̋Ȃ��\�񂳂�Ă���Ό��݂�BGM���t�F�[�h�A�E�g�����Ă���Đ�����
	if (fade_state == Fade_State::Playing)
	{
		fade_state = Fade_State::Fade_Out;
	}
	next_bgm = name;
}

bool Sound_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Sound_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_DragFloat(u8"�J�ڎ���", "##fade_time", &fade_time, window_center);
		ImGui::LeftText_DragFloat(u8"BGM����", "##bgm_volume", &bgm_volume, window_center);
	}
	return true;
}