#include "muzzle.h"
using namespace std;
void Muzzle::Awake()
{

}

void Muzzle::Start()
{
	shotpoint = GameObject::Find("shotpoint");
	shot=transform->GetComponent<Particle>();

}

void Muzzle::Update()
{
	transform->Set_rotation(transform->LookAt(shotpoint.lock()->transform->Get_position()));
	if (Input_Manager::mouse_tracker.rightButton == Input_Manager::mouse_tracker.PRESSED)
	{

		shot.lock()->Play();
	}
}

bool Muzzle::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Muzzle"))
	{

		bool removed = true;
		if (ImGui::BeginPopupContextItem("Muzzle_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<Muzzle>(shared_from_this()));
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