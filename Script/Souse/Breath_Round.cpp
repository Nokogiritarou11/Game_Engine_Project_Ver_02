#include "Breath_Round.h"
#include "ToPlayer_Collider.h"
#include "Actions.h"
using namespace std;

void Breath_Round::OnEnable()
{
	if (!col.expired())
	{
		col.lock()->Size = 10;
	}
}

void Breath_Round::Start()
{
	col = GetComponent<ToPlayer_Collider>();
}

void Breath_Round::Update()
{
	transform->Set_position(transform->Get_position() - transform->Get_forward() * 1000.0f * Time::deltaTime );
	col.lock()->Size += 600.0f * Time::deltaTime;
}

bool Breath_Round::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Breath_Round");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Breath_Round_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Breath_Round>(shared_from_this()));
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
	}

	return true;
}