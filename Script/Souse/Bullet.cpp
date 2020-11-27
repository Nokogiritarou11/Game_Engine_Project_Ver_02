#include "Bullet.h"
using namespace std;

void Bullet::OnEnable()
{
	timer = 0;
}

void Bullet::Update()
{
	transform->Set_position(transform->Get_position() + transform->Get_forward() * speed * Time::deltaTime);
	timer += Time::deltaTime;
	if (timer > 10)
	{
		Destroy(gameObject);
	}
}

bool Bullet::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Bullet");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Bullet_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Bullet>(shared_from_this()));
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