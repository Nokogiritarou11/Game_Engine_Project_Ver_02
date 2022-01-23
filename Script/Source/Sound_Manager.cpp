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
		//予約された曲があればフェードインから再生開始
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
			//フェードイン
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
			//フェードアウト
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
	//次の曲が予約されていれば現在のBGMをフェードアウトさせてから再生する
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
		ImGui::LeftText_DragFloat(u8"遷移時間", "##fade_time", &fade_time, window_center);
		ImGui::LeftText_DragFloat(u8"BGM音量", "##bgm_volume", &bgm_volume, window_center);
	}
	return true;
}