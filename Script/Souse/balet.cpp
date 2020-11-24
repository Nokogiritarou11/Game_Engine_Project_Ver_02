#include "Balet.h"
using namespace std;
void Balet::Awake()
{

}

void Balet::Start()
{
	muzzle = GameObject::Find("muzzle");

	//Vector3 angle=;
	timer = 0;
	transform->Set_localRotation(muzzle.lock()->transform->Get_rotation());
	transform->Set_localPosition( muzzle.lock()->transform->Get_position());
}

void Balet::Update()
{
	transform->Set_position(transform->Get_position() + transform->Get_forward());
	timer++;
	if (timer > 120)
	{
		Destroy(gameObject);
	}
}

bool Balet::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Balet"))
	{

		bool removed = true;
		if (ImGui::BeginPopupContextItem("Balet_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<Balet>(shared_from_this()));
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