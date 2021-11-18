#include "Behaviour.h"
#include "Include_ImGui.h"

using namespace BeastEngine;
using namespace std;

Behaviour::Behaviour()
{
	enabled_old = enabled;
}

void Behaviour::Set_Enabled(const bool value)
{
	if (value != enabled_old)
	{
		enabled = value;
		enabled_old = value;
		Set_Active(value);
	}
}

bool Behaviour::Get_Enabled() const
{
	return enabled;
}

bool Behaviour::Draw_ImGui_Header(const string& component_name, bool& open)
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	open = ImGui::CollapsingHeader(component_name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	string sub = component_name + "_sub";
	if (ImGui::BeginPopupContextItem(component_name.c_str()))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Destroy(dynamic_pointer_cast<Component>(shared_from_this()));
			removed = true;
		}
		ImGui::EndPopup();
	}

	if (removed) return false;

	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 20.0f);
	bool enable = Get_Enabled();
	if (ImGui::Checkbox("##enable", &enable))
	{
		Set_Enabled(enable);
	}

	return true;
}