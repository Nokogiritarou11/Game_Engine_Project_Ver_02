#include "UI_Health_Bar.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void UI_Health_Bar::Awake()
{
	fill_sprite = transform->Find("Fill_Sprite").lock()->Get_Component<Sprite_Renderer>();
	fill_max = fill_sprite.lock()->size.x;
}

void UI_Health_Bar::Set_Parameter(const weak_ptr<Character_Parameter>& character_parameter)
{
	parameter = character_parameter;
	health_old = parameter.lock()->hp;
}

void UI_Health_Bar::LateUpdate()
{
	const auto& param = parameter.lock();

	if(param->hp != health_old)
	{
		fill_sprite.lock()->size.x = fill_max * (param->hp / param->max_hp);
	}
}

bool UI_Health_Bar::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("UI_Health_Bar", open)) return false;

	if (open)
	{
		//const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"設定可能なパラメータはありません");
	}
	return true;
}