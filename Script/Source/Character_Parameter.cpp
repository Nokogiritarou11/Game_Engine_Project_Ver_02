#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Character_Parameter::Awake()
{
	hp = max_hp;
}

bool Character_Parameter::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Character_Parameter", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat("Max_HP", "##Max_HP", &max_hp, window_center, -FLT_MIN, 1.0f);

		ImGui::Text("HP");
		ImGui::SameLine(window_center);
		ImGui::Text("%01f", hp);

		ImGui::LeftText_Checkbox(u8"�C�x���g��", "##eventing", &eventing, window_center);
		ImGui::LeftText_Checkbox(u8"������", "##living", &living, window_center);
		ImGui::LeftText_Checkbox(u8"�ړ���", "##moving", &moving, window_center);
		ImGui::LeftText_Checkbox(u8"�U����", "##attacking", &attacking, window_center);
		ImGui::LeftText_Checkbox(u8"���", "##dodging", &dodging, window_center);
		ImGui::LeftText_Checkbox(u8"��_����", "##damaging", &damaging, window_center);
		ImGui::LeftText_Checkbox(u8"�ڒn��", "##is_ground", &is_ground, window_center);
	}
	return true;
}