#include "UI_Manager.h"
#include "UI_Health_Bar.h"

using namespace std;
using namespace BeastEngine;

void UI_Manager::Activate_Enemy_Health_Bar(const std::weak_ptr<Character_Parameter>& character_parameter) const
{
	const auto& bar = transform->Find("Enemy_Health_Bar").lock()->Get_Component<UI_Health_Bar>();
	bar->gameobject->Set_Active(true);
	bar->Set_Parameter(character_parameter);
}

void UI_Manager::Activate_Player_Health_Bar(const std::weak_ptr<Character_Parameter>& character_parameter) const
{
	const auto& bar = transform->Find("Player_Health_Bar").lock()->Get_Component<UI_Health_Bar>();
	bar->gameobject->Set_Active(true);
	bar->Set_Parameter(character_parameter);
}

bool UI_Manager::Draw_ImGui()
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