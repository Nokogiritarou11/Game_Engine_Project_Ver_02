#include "Muzzle.h"
using namespace std;
void Muzzle::Awake()
{
}

void Muzzle::Start()
{
	shot_point = GameObject::Find("Shot_Point");
}

void Muzzle::Update()
{
	transform->Set_rotation(transform->LookAt(shot_point.lock()->transform->Get_position()));
}

bool Muzzle::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Muzzle");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Muzzle_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Muzzle>(shared_from_this()));
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