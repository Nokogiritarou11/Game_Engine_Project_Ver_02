#include "Player_Damageable.h"

#include "Character_Hit_Stop_Manager.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Player_Damageable::Awake()
{
	animator = Get_Component<Animator>();
	parameter = Get_Component<Character_Parameter>();
	hit_stop_manager = Get_Component<Character_Hit_Stop_Manager>();
}

bool Player_Damageable::Take_Damage(const int damage_hp, const int damage_stun, const shared_ptr<Transform>& from_transform, const Damage_Type damage_state)
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	const Vector3 from_pos = from_transform->Get_Position();
	transform->Set_Local_Rotation(transform->Look_At(Vector3(from_pos.x, transform->Get_Position().y, from_pos.z)));
	hit_stop_manager.lock()->Start_Hit_Stop(0.05f);

	if (param->guarding)
	{
		if (param->just_guarding)
		{
			anim->Set_Trigger("Parry");
			from_transform->Set_Local_Position(transform->Get_Position() + transform->Get_Forward() * 2.25f);
		}
		else
		{
			anim->Set_Trigger("Damage");
		}
		return false;
	}

	param->hp -= damage_hp;
	anim->Set_Trigger("Damage");
	anim->Set_Int("Damage_State", static_cast<int>(damage_state));
	return true;
}

bool Player_Damageable::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Player_Damageable", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"İ’è‚Å‚«‚éƒpƒ‰ƒ[ƒ^‚Í‚ ‚è‚Ü‚¹‚ñ");
	}
	return true;
}