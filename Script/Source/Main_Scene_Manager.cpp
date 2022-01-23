#include "Main_Scene_Manager.h"
#include "Sound_Manager.h"
#include "Scene_Change_Manager.h"
using namespace std;
using namespace BeastEngine;

void Main_Scene_Manager::Awake()
{
	GameObject::Find("Sound_Manager").lock()->Get_Component<Sound_Manager>()->Play_BGM(BGM_Name::Normal_Battle);
	clear_sprite = transform->Find("Clear_Sprite").lock()->Get_Component<Sprite_Renderer>();
	over_sprite = transform->Find("Over_Sprite").lock()->Get_Component<Sprite_Renderer>();
}

void Main_Scene_Manager::Update()
{
	if (state == Game_State::Game_Over || state == Game_State::Game_Clear)
	{
		wait_timer += Time::delta_time;
		if (wait_timer >= wait_time)
		{
			fade_timer += Time::delta_time;
			const auto& fade = fade_sprite.lock();

			//暗転用テクスチャをフェードインさせる
			if (const float rate = fade_timer / fade_time; rate < 1)
			{
				fade->color.w = Mathf::Lerp(0, 1, rate);
			}
			else
			{
				fade->color.w = 1;
				fade_timer = 0;
				wait_timer = 0;
				state = Game_State::Scene_Change;
			}
		}
	}
	else if (state == Game_State::Scene_Change)
	{
		wait_timer += Time::delta_time;
		if (wait_timer >= end_time)
		{
			wait_timer = 0;
			state = Game_State::Playing;
			GameObject::Find("Scene_Change_Manager").lock()->Get_Component<Scene_Change_Manager>()->Change_Scene("Assets\\Scene\\Title.bin");
		}
	}
}

void Main_Scene_Manager::Game_Over(const float delay_time)
{
	state = Game_State::Game_Over;
	wait_time = delay_time;
	fade_sprite = over_sprite;
}

void Main_Scene_Manager::Game_Clear(const float delay_time)
{
	state = Game_State::Game_Clear;
	wait_time = delay_time;
	fade_sprite = clear_sprite;
}

bool Main_Scene_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Main_Scene_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_DragFloat(u8"遷移時間", "##fade_time", &fade_time, window_center);
		ImGui::LeftText_DragFloat(u8"終了待機時間", "##end_time", &end_time, window_center);
	}
	return true;
}