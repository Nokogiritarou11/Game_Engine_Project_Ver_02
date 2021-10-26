#include "Enemy_Normal_01_State_Setter.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Normal_01_State_Setter::Awake()
{
	animator = Get_Component<Animator>();
	parameter = Get_Component<Character_Parameter>();
	target_transform = GameObject::Find_With_Tag("player").lock()->transform;
}

void Enemy_Normal_01_State_Setter::Set_State()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();
	const auto& target = target_transform.lock();

	if (!param->attacking && !param->damaging)
	{
		anim->Set_Bool("Move", true);
	}
	if (param->moving)
	{
		if (Vector3::DistanceSquared(target->Get_Position(), transform->Get_Position()) < attack_distance * attack_distance)
		{
			anim->Set_Trigger("Attack");
			anim->Set_Bool("Move", false);
		}
	}
}

bool Enemy_Normal_01_State_Setter::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Normal_01_State_Setter", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_DragFloat(u8"UŒ‚”»’è‹——£", "##Attack_Distance", &attack_distance, window_center);
	}
	return true;
}