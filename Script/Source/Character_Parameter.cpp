#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Character_Parameter::Awake()
{
	HP = Max_HP;
}

bool Character_Parameter::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Character_Parameter", open)) return false;

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat("Max_HP", "##Max_HP", &Max_HP, window_center, -FLT_MIN, 1.0f);

		ImGui::Text("HP");
		ImGui::SameLine(window_center);
		ImGui::Text("%01f", HP);

		ImGui::LeftText_Checkbox(u8"イベント中", "##eventing", &eventing, window_center);
		ImGui::LeftText_Checkbox(u8"生存中", "##living", &living, window_center);
		ImGui::LeftText_Checkbox(u8"移動中", "##moveing", &moveing, window_center);
		ImGui::LeftText_Checkbox(u8"攻撃中", "##attacking", &attacking, window_center);
		ImGui::LeftText_Checkbox(u8"回避中", "##dodging", &dodging, window_center);
		ImGui::LeftText_Checkbox(u8"被ダメ中", "##damaging", &damaging, window_center);
		ImGui::LeftText_Checkbox(u8"接地中", "##is_ground", &is_ground, window_center);
	}
	return true;
}