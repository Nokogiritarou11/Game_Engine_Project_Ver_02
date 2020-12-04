#include "ToPlayer_Collider.h"
#include "Player.h"
using namespace std;

void ToPlayer_Collider::OnEnable()
{
	Trigger = false;
}

void ToPlayer_Collider::Start()
{
	player = GameObject::Find("Player").lock()->GetComponent<Player>();
}

void ToPlayer_Collider::Update()
{
	if (!Trigger && Vector3::Distance(transform->Get_position(), player.lock()->transform->Get_position()) <= Size)
	{
		player.lock()->Damage(Damage);
		Trigger = true;
	}
}

bool ToPlayer_Collider::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("ToPlayer_Collider");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("ToPlayer_Collider_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<ToPlayer_Collider>(shared_from_this()));
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
		ImGui::InputInt("Damage", &Damage);
		ImGui::InputFloat("Size", &Size);
	}

	return true;
}