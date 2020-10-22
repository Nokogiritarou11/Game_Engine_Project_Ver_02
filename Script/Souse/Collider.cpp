#include "Collider.h"
#include "Include_Mono.h"
#include "Include_ImGui.h"
#include "Stage_Manager.h"
#include "Player.h"

void Collider::Start()
{
	GameObject::Find("Player").lock()->GetComponent<Player>()->colliders.push_back(static_pointer_cast<Collider>(shared_from_this()));
	Disable_Line = -75;
}

void Collider::OnEnable()
{
	if (obj_type != None && obj_type != Block)
	{
		transform->Set_eulerAngles(0, 0, 0);
		turn_y = 0;
	}
}

void Collider::Update()
{
	if (transform->Get_position().z < Disable_Line)
	{
		gameObject->SetActive(false);
	}
	if (obj_type != None && obj_type != Block)
	{
		turn_y += Time::deltaTime * 150;
		transform->Set_eulerAngles(0, turn_y, 0);
	}
}

bool Collider::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Collider"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Collider_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<Collider>(shared_from_this()));
				removed = false;
			}
			ImGui::EndPopup();
			if (!removed)
			{
				return false;
			}
		}
	}
	return true;
}