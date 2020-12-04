#include "Enemy_Collider.h"
#include "Dragon_HP.h"
using namespace std;

void Enemy_Collider::Start()
{
	GameObject::Find("Dragon").lock()->GetComponent<Dragon_HP>()->colliders.emplace_back(dynamic_pointer_cast<Enemy_Collider>(shared_from_this()));
}

bool Enemy_Collider::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Enemy_Collider");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Enemy_Collider_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Enemy_Collider>(shared_from_this()));
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
		ImGui::InputFloat("Damage_Magnification", &Damage_Magnification);
		ImGui::InputFloat("Size", &Size);
	}

	return true;
}