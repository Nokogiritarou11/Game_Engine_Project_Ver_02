#include "Dragon_HP.h"
#include "doragon.h"
#include "Dragon_Animation_Event.h"
using namespace std;

void Dragon_HP::Awake()
{
	HP = 3500;
}

void Dragon_HP::Update()
{

}

void Dragon_HP::Damage(int damage)
{
	HP -= damage;

	if (HP < 0)
	{
		HP = 0;
		colliders.clear();
		GetComponent<Doragon>()->SetEnabled(false);
		GetComponent<Dragon_Animation_Event>()->SetEnabled(false);
		GetComponent<Animator>()->Stop();
		GetComponent<Animator>()->animation_speed = 1.0f;
		GetComponent<Animator>()->Play(8);
	}
}

bool Dragon_HP::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Dragon_HP");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Dragon_HP_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Dragon_HP>(shared_from_this()));
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
		ImGui::InputInt("HP", &HP);
	}

	return true;
}