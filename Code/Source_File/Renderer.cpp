#include "Renderer.h"
#include "Include_ImGui.h"

using namespace std;
using namespace BeastEngine;

void Renderer::Set_Enabled(bool value)
{
	if (value != enabled_old)
	{
		enabled = value;
		enabled_old = value;
		Set_Active(value);
	}
}

bool Renderer::Get_Enabled()
{
	return enabled;
}

bool Renderer::Draw_ImGui_Header(string component_name, bool& open)
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	open = ImGui::CollapsingHeader(component_name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	string sub = component_name + "_sub";
	if (ImGui::BeginPopupContextItem(component_name.c_str()))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Component>(shared_from_this()));
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