#include "Enemy_Normal_01_Damageable.h"
#include "Character_Hit_Stop_Manager.h"
#include "Enemy_Parameter.h"
#include "Enemy_Manager.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Normal_01_Damageable::Awake()
{
	animator = Get_Component<Animator>();
	parameter = Get_Component<Enemy_Parameter>();
	hit_stop_manager = Get_Component<Character_Hit_Stop_Manager>();

	const auto& manager = GameObject::Find_With_Tag("Game_Manager").lock();
	enemy_manager = manager->Get_Component<Enemy_Manager>();
}

bool Enemy_Normal_01_Damageable::Take_Damage(const int damage_hp, const int damage_stun, const shared_ptr<Transform>& from_transform, const Damage_Type damage_state)
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	const Vector3 now_pos = transform->Get_Position();
	Vector3 look_pos = now_pos - from_transform->Get_Forward();
	look_pos.y = now_pos.y;
	transform->Set_Local_Rotation(transform->Look_At(look_pos));

	hit_stop_manager.lock()->Start_Hit_Stop(0.05f);
	enemy_manager.lock()->last_attack_target = parameter;
	param->last_damaged_timer = 0;

	if (param->guarding)
	{
		anim->Set_Trigger("Damage");
		return false;
	}

	if (param->hp > 0)
	{
		param->hp -= damage_hp;

		if (param->is_ground && !param->stunning)
		{
			param->stun -= damage_stun;
		}

		if (param->hp <= 0)
		{
			anim->Set_Trigger("Dead");
			anim->Set_Trigger("Damage");
			anim->Set_Int("Damage_State", static_cast<int>(damage_state));
			enemy_manager.lock()->Enemy_Dead(true, parameter);
			return true;
		}

		if (param->stun <= 0)
		{
			param->stunning = true;
			anim->Set_Trigger("Stun");
			enemy_manager.lock()->Enemy_Stunned(true, parameter);
			return true;
		}
	}

	anim->Set_Trigger("Damage");
	anim->Set_Int("Damage_State", static_cast<int>(damage_state));
	return true;
}

bool Enemy_Normal_01_Damageable::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Normal_01_Damageable", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"İ’è‚Å‚«‚éƒpƒ‰ƒ[ƒ^‚Í‚ ‚è‚Ü‚¹‚ñ");
	}
	return true;
}