#include "Enemy_Normal_01_State_Setter.h"
#include "Character_Parameter.h"
#include "Enemy_Manager.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Normal_01_State_Setter::Awake()
{
	animator = Get_Component<Animator>();
	parameter = Get_Component<Character_Parameter>();
	target_transform = GameObject::Find_With_Tag("player").lock()->transform;
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
}

void Enemy_Normal_01_State_Setter::Set_State()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();
	const auto& target = target_transform.lock();

	if (!param->attacking && !param->damaging && !param->stunning)
	{
		anim->Set_Bool("Move", true);
	}

	if (param->moving)
	{
		if (param->attacking || param->damaging || param->stunning)
		{
			anim->Set_Bool("Move", false);
		}

		if (Vector3::DistanceSquared(target->Get_Position(), transform->Get_Position()) < powf(attack_distance, 2))
		{
			anim->Set_Trigger("Attack");
			anim->Set_Bool("Move", false);
		}
	}

	if (const bool attack_preliminary = anim->Get_Bool("Attack_Preliminary"); param->is_attack_preliminary != attack_preliminary)
	{
		param->is_attack_preliminary = attack_preliminary;
		if (param->is_attack_preliminary)
		{
			enemy_manager.lock()->Add_Attacking_List(parameter);
		}
		else
		{
			enemy_manager.lock()->Remove_Attacking_List(parameter);
		}
	}

	if (anim->Get_Bool("Stun_End"))
	{
		anim->Set_Bool("Stun_End", false);
		param->stunning = false;
		enemy_manager.lock()->Remove_Stunning_List(parameter);
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