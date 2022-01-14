#include "UI_Smash.h"
#include "Enemy_Parameter.h"

using namespace std;
using namespace BeastEngine;

void UI_Smash::Awake()
{
	sprite_main = transform->Get_Child(0).lock()->Get_Component<Sprite_Renderer>();
}

void UI_Smash::Set_State(const int value)
{
	if (state != value)
	{
		state = value;
		switch (state)
		{
			case 0:
				gameobject->Set_Active(true);
				sprite_main.lock()->color = start_color;
				transform->Set_Scale(Vector3(1, 1, 1));
				timer = 0;
				break;
			case 1:
				is_playing = true;
				break;
			case 2:
				if(!is_playing)	gameobject->Set_Active(false);
				break;
			default:
				break;
		}
	}
}

void UI_Smash::LateUpdate()
{
	if (is_playing)
	{
		const auto& sprite = sprite_main.lock();
		timer += Time::delta_time;
		const float rate = timer / inactive_time;

		sprite->color = Vector4::Lerp(start_color, end_color, rate);
		transform->Set_Scale(Vector3::Lerp(Vector3(1, 1, 1), Vector3(end_scale, end_scale, 1), rate));

		if (rate >= 1)
		{
			is_playing = false;
			Set_State(2);
		}
	}

}

bool UI_Smash::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("UI_Smash", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_DragFloat(u8"消失時間", "##inactive_time", &inactive_time, window_center);
		ImGui::LeftText_ColorEdit4(u8"初期色", "##default_color", start_color, window_center);
		ImGui::LeftText_ColorEdit4(u8"消失時色", "##end_color", end_color, window_center);
		ImGui::LeftText_DragFloat(u8"消失時サイズ", "##end_scale", &end_scale, window_center);
	}
	return true;
}
