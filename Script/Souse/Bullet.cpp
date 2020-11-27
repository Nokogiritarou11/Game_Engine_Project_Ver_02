#include "Bullet.h"
using namespace std;
void Bullet::Awake()
{
}

void Bullet::Start()
{
	muzzle = GameObject::Find("muzzle");

	//Vector3 angle=;
	timer = 0;
	transform->Set_localRotation(muzzle.lock()->transform->Get_rotation());
	transform->Set_localPosition(muzzle.lock()->transform->Get_position());
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
	if (ImGui::CollapsingHeader("Bullet"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Bullet_sub"))
		{
			if (ImGui::Selectable(u8"�R���|�[�l���g���폜"))
			{
				Object::Destroy(dynamic_pointer_cast<Bullet>(shared_from_this()));
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