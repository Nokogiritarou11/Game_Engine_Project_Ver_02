#include "Enemy_Normal_01_Damageable.h"
#include "Character_Hit_Stop_Manager.h"
#include "Character_Parameter.h"
#include "Enemy_Manager.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Normal_01_Damageable::Awake()
{
	animator = Get_Component<Animator>();
	parameter = Get_Component<Character_Parameter>();
	hit_stop_manager = Get_Component<Character_Hit_Stop_Manager>();

	const auto& manager = GameObject::Find_With_Tag("Game_Manager").lock();
	enemy_manager = manager->Get_Component<Enemy_Manager>();
}

bool Enemy_Normal_01_Damageable::Take_Damage(const int damage_hp, const int damage_stun, const shared_ptr<Transform>& from_transform, const Damage_Type damage_state)
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	const Vector3 from_pos = from_transform->Get_Position();
	transform->Set_Local_Rotation(transform->Look_At(Vector3(from_pos.x, transform->Get_Position().y, from_pos.z)));

	hit_stop_manager.lock()->Start_Hit_Stop(0.05f);
	enemy_manager.lock()->last_attack_target = parameter;

	if (param->guarding)
	{
		anim->Set_Trigger("Damage");
		return false;
	}

	param->hp -= damage_hp;

	if (param->is_ground && !param->stunning)
	{
		param->stun -= damage_stun;
	}

	if (param->hp <= 0)
	{
		param->living = false;
		anim->Set_Trigger("Death");
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