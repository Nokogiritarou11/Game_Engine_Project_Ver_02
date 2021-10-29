#include "Player_Damageable.h"
#include "Character_Hit_Stop_Manager.h"
#include "Character_Parameter.h"
#include "Object_Pool.h"
#include "Time_Manager.h"

using namespace std;
using namespace BeastEngine;

void Player_Damageable::Awake()
{
	animator = Get_Component<Animator>();
	parameter = Get_Component<Character_Parameter>();
	pool = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Object_Pool>();
	hit_stop_manager = Get_Component<Character_Hit_Stop_Manager>();
	time_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Time_Manager>();
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
			time_manager.lock()->Start_Time_Slow(0.05f, 0.1f, 0.05f);
			pool.lock()->Instance_In_Pool(guard_particle_key, transform->Get_Position() + transform->Get_Forward() * 0.7f + Vector3(0, 1.2f, 0), transform->Get_Local_Rotation());
		}
		else
		{
			anim->Set_Trigger("Damage");
			pool.lock()->Instance_In_Pool(guard_particle_key, transform->Get_Position() + transform->Get_Forward() * 0.25f + Vector3(0, 1.1f, 0), transform->Get_Local_Rotation());
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
		ImGui::LeftText_InputText(u8"ガードエフェクトキー", "##hit_particle_key", &guard_particle_key, window_center);
	}
	return true;
}