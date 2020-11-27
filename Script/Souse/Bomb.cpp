#include "Bomb.h"
using namespace std;
void Bomb::Awake()
{
}

void Bomb::Start()
{
	//Vector3 angle=;
	timer = 0;
	power = 0;
}

void Bomb::Update()
{
	transform->Set_position(transform->Get_position() + transform->Get_forward() + Vector3(0, power, 0));
	timer += Time::deltaTime;
	power -= 1.0f * Time::deltaTime;
	if (timer > 10)
	{
		Destroy(gameObject);
	}
}

bool Bomb::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Bomb"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Bomb_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<Bomb>(shared_from_this()));
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