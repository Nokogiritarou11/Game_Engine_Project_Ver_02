#include "doragon.h"
#include "A-Star.h"
using namespace std;

void Doragon::Awake()
{
}

void Doragon::Start()
{
	

}

void Doragon::Update()
{

}

bool Doragon::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Doragon"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Doragon_sub"))
		{

			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
			
				Object::Destroy(dynamic_pointer_cast<Doragon>(shared_from_this()));
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