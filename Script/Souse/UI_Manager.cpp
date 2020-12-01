#include "UI_Manager.h"
using namespace std;

void UI_Manager::Start()
{
	UI_Cursor = GameObject::Find("Cursor").lock()->GetComponent<Sprite_Renderer>();
	shared_ptr<Sprite_Renderer> ui_cursor = UI_Cursor.lock();
	ui_cursor->transform->Set_position(static_cast<float>(Screen::Get_Width()) / 2 - ui_cursor->Size.x / 2, static_cast<float>(Screen::Get_Height()) / 2 - ui_cursor->Size.y / 2, 0);
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