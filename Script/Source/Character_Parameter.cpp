#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Character_Parameter::Awake()
{
	HP = Max_HP;
}

bool Character_Parameter::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Character_Parameter", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	if (ImGui::BeginPopupContextItem("Character_Parameter_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Character_Parameter>(shared_from_this()));
			removed = true;
		}
		ImGui::EndPopup();
	}
	if (removed)
	{
		return false;
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 20.0f);
	bool enable = Get_Enabled();
	if (ImGui::Checkbox("##enable", &enable))
	{
		Set_Enabled(enable);
	}

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.6f;

		ImGui::Text("Max_HP");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat("##Max_HP", &Max_HP, 1.0f);

		ImGui::Text("HP");
		ImGui::SameLine(window_center);
		ImGui::Text("%01f", HP);

		ImGui::Text("Eventing");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Checkbox("##Eventing", &Eventing);

		ImGui::Text("Living");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Checkbox("##Living", &Living);

		ImGui::Text("Moveing");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Checkbox("##Moveing", &Moveing);

		ImGui::Text("Attacking");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Checkbox("##Attacking", &Attacking);

		ImGui::Text("Dodging");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Checkbox("##Dodging", &Dodging);

		ImGui::Text("Damaging");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::Checkbox("##Damaging", &Damaging);
	}
	return true;
}