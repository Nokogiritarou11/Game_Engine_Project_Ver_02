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
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
	enemy_manager.lock()->Add_Enemy_List(gameobject);
	hit_stop_manager = Get_Component<Character_Hit_Stop_Manager>();
}

bool Enemy_Normal_01_Damageable::Take_Damage(const int damage_hp, const int damage_stun, const shared_ptr<Transform>& from_transform, const Damage_Type damage_state)
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	const Vector3 from_pos = from_transform->Get_Position();
	transform->Set_Local_Rotation(transform->Look_At(Vector3(from_pos.x, transform->Get_Position().y, from_pos.z)));
	hit_stop_manager.lock()->Start_Hit_Stop(0.05f);

	if (param->guarding)
	{
		anim->Set_Trigger("Damage");
		return false;
	}

	param->hp -= damage_hp;
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
		ImGui::Text(u8"設定できるパラメータはありません");
	}
	return true;
}