#include "Muzzle.h"
using namespace std;
void Muzzle::Awake()
{
}

void Muzzle::Start()
{
	shotpoint = GameObject::Find("shotpoint");
	muzzleflash = gameObject->GetComponent<Particle>();
}

void Muzzle::Update()
{
	transform->Set_rotation(transform->LookAt(shotpoint.lock()->transform->Get_position()));
	if (Input::GetMouseButtonDown(0))

	{
		muzzleflash.lock()->Play();
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