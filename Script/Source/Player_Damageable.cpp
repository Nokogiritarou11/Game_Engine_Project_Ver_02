#include "Player_Damageable.h"
#include "Character_Hit_Stop_Manager.h"
#include "Player_Parameter.h"
#include "Player_Camera_Controller.h"
#include "Object_Pool.h"
#include "Time_Manager.h"

using namespace std;
using namespace BeastEngine;

void Player_Damageable::Awake()
{
	animator = Get_Component<Animator>();
	parameter = Get_Component<Player_Parameter>();
	camera_controller = GameObject::Find_With_Tag("main_camera").lock()->transform->Get_Parent().lock()->Get_Component<Player_Camera_Controller>();
	pool = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Object_Pool>();
	hit_stop_manager = Get_Component<Character_Hit_Stop_Manager>();
	time_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Time_Manager>();
}

void Player_Damageable::Update()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	param->is_invincible = anim->Get_Bool("Invincible");
}

bool Player_Damageable::Take_Damage(const int damage_hp, const int damage_stun, const shared_ptr<Transform>& from_transform, const Damage_Type damage_state)
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	if (param->is_invincible) return false;

	const Vector3 from_pos = from_transform->Get_Position();
	transform->Set_Local_Rotation(transform->Look_At(Vector3(from_pos.x, transform->Get_Position().y, from_pos.z)));
	hit_stop_manager.lock()->Start_Hit_Stop(0.05f);

	if (param->guarding)
	{
		if (param->just_guarding)
		{
			anim->Set_Trigger("Parry");
			from_transform->Set_Local_Position(transform->Get_Position() + transform->Get_Forward() * 2.0f);
			time_manager.lock()->Start_Time_Slow(parry_time_stop_delay, parry_time_stop_time, parry_time_stop_speed);
			camera_controller.lock()->Shake_Camera(parry_shake_camera_count, parry_shake_camera_power);
			pool.lock()->Instance_In_Pool(parry_particle_key, transform->Get_Position() + transform->Get_Right() * parry_particle_position.x + transform->Get_Up() * parry_particle_position.y + transform->Get_Forward() * parry_particle_position.z, transform->Get_Local_Rotation());
		}
		else
		{
			anim->Set_Trigger("Damage");
			camera_controller.lock()->Shake_Camera(guard_shake_camera_count, guard_shake_camera_power);
			pool.lock()->Instance_In_Pool(guard_particle_key, transform->Get_Position() + transform->Get_Right() * guard_particle_position.x + transform->Get_Up() * guard_particle_position.y + transform->Get_Forward() * guard_particle_position.z, transform->Get_Local_Rotation());
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

		if (ImGui::TreeNode(u8"パリィ時演出パラメータ"))
		{
			if (ImGui::TreeNode(u8"スロー効果"))
			{
				ImGui::LeftText_DragFloat(u8"発生ディレイ", "##parry_time_stop_delay", &parry_time_stop_delay, window_center);
				ImGui::LeftText_DragFloat(u8"効果時間", "##parry_time_stop_time", &parry_time_stop_time, window_center);
				ImGui::LeftText_DragFloat(u8"速度", "##parry_time_stop_speed", &parry_time_stop_speed, window_center);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"カメラ揺れ"))
			{
				ImGui::LeftText_DragInt(u8"回数", "##parry_shake_camera_count", &parry_shake_camera_count, window_center);
				ImGui::LeftText_DragFloat(u8"強度", "##parry_shake_camera_power", &parry_shake_camera_power, window_center);
				ImGui::TreePop();
			}

			ImGui::LeftText_DragFloat3(u8"エフェクト座標", "##parry_particle_position", parry_particle_position, window_center);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode(u8"ガード時演出パラメータ"))
		{
			if (ImGui::TreeNode(u8"カメラ揺れ"))
			{
				ImGui::LeftText_DragInt(u8"回数", "##guard_shake_camera_count", &guard_shake_camera_count, window_center);
				ImGui::LeftText_DragFloat(u8"強度", "##guard_shake_camera_power", &guard_shake_camera_power, window_center);
				ImGui::TreePop();
			}

			ImGui::LeftText_DragFloat3(u8"エフェクト座標", "##guard_particle_position", guard_particle_position, window_center);

			ImGui::TreePop();
		}

		ImGui::LeftText_InputText(u8"ガードエフェクトキー", "##guard_particle_key", &guard_particle_key, window_center);
		ImGui::LeftText_InputText(u8"パリィエフェクトキー", "##parry_particle_key", &parry_particle_key, window_center);
	}
	return true;
}