#include "Auto_Disable.h"
using namespace std;

void  Auto_Disable::OnEnable()
{
	timer = 0;
}

void  Auto_Disable::Update()
{
	timer += Time::deltaTime;
	if (timer >= Disable_Time)
	{
		gameObject->SetActive(false);
	}
}

bool  Auto_Disable::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader(" Auto_Disable");

	bool removed = true;
	if (ImGui::BeginPopupContextItem(" Auto_Disable_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Auto_Disable>(shared_from_this()));
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
		ImGui::DragFloat("Disable_Time", &Disable_Time, 0.1f, 0.0f, FLT_MAX);
	}

	return true;
}