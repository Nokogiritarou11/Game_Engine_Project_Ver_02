#include "Scroll.h"
#include "Include_Mono.h"
#include "Include_ImGui.h"
#include "Stage_Manager.h"
using namespace std;

void Scroll::Start()
{
	stage_manager = GameObject::Find("Stage_Manager").lock()->GetComponent<Stage_Manager>();
	scroll_speed = stage_manager.lock()->Scroll_Speed;
	Debug::Log(gameObject->name);
}

void Scroll::Update()
{
	scroll_speed = stage_manager.lock()->Scroll_Speed;
	transform->Set_position(transform->Get_position() + Vector3(0, 0, -scroll_speed * Time::deltaTime));
}

bool Scroll::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Scroll"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Scroll_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<Scroll>(shared_from_this()));
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