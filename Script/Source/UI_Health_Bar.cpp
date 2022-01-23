#include "UI_Health_Bar.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void UI_Health_Bar::Awake()
{
	bar_main.sprite = transform->Find("Fill_Sprite").lock()->Get_Component<Sprite_Renderer>();
	bar_main.max = bar_main.sprite.lock()->size.x;

	bar_delay.sprite = transform->Find("Delay_Sprite").lock()->Get_Component<Sprite_Renderer>();
}

void UI_Health_Bar::Set_Parameter(const weak_ptr<Character_Parameter>& character_parameter)
{
	parameter = character_parameter;
	bar_main.old = parameter.lock()->hp;
}

void UI_Health_Bar::LateUpdate()
{
	//ゲージの長さの更新
	if(const auto& param = parameter.lock(); param->hp != bar_main.old)
	{
		bar_main.old = param->hp;
		bar_main.sprite.lock()->size.x = bar_main.max * (param->hp / param->max_hp);
		delay_timer = delay_start_time;
	}

	//少し遅れて消える赤ゲージ
	if(delay_timer > 0)
	{
		delay_timer -= Time::delta_time;
		if(delay_timer <= 0)
		{
			delay_timer = 0;
			bar_delay.sprite.lock()->size.x = bar_main.sprite.lock()->size.x;
		}
	}
}

bool UI_Health_Bar::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("UI_Health_Bar", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_DragFloat(u8"赤ゲージ待ち時間", "##delay_start_time", &delay_start_time, window_center);
	}
	return true;
}