#include "Bomb.h"
using namespace std;

void Bomb::Update()
{
	transform->Set_position(transform->Get_position() + transform->Get_forward() * speed * Time::deltaTime);
	Vector3 e = transform->Get_eulerAngles();
	transform->Set_eulerAngles(e.x + 30.0f * Time::deltaTime, e.y, e.z);
}

bool Bomb::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Bomb");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Bomb_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Bomb>(shared_from_this()));
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
		ImGui::DragFloat("speed", &speed, 0.1f, 0.0f, FLT_MAX);
	}
	return true;
}