#include "UI_Manager.h"
using namespace std;

void UI_Manager::Start()
{

}

void UI_Manager::Update()
{

}

bool UI_Manager::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("UI_Manager");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("UI_Manager_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<UI_Manager>(shared_from_this()));
			removed = false;
		}
		ImGui::EndPopup();
	}
	if (!removed)
	{
		return false;
	}

	if (open)
	{
	}
	return true;
}