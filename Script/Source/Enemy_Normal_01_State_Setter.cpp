#include "Enemy_Normal_01_State_Setter.h"
#include "Enemy_Parameter.h"
#include "Editor.h"
#include "Enemy_Manager.h"
#include "Engine.h"
#include "Object_Pool.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Normal_01_State_Setter::Awake()
{
	animator = Get_Component<Animator>();
	parameter = Get_Component<Enemy_Parameter>();
	target_transform = GameObject::Find_With_Tag("player").lock()->transform;
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
	pool = enemy_manager.lock()->Get_Component<Object_Pool>();
}

void Enemy_Normal_01_State_Setter::OnEnable()
{
	wait_state = Wait_State::Chase;
	wait_distance = Mathf::Random_Range(wait_distance_min, wait_distance_max);
	wait_state_timer = Mathf::Random_Range(state_change_time_min, state_change_time_max);
	attack_mode_old = false;
}

void Enemy_Normal_01_State_Setter::Set_State()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();
	const auto& target = target_transform.lock();

	if (param->attacking || param->damaging || param->stunning)
	{
		anim->Set_Bool("Move", false);
		wait_state = Wait_State::Wait;
		wait_state_timer = Mathf::Random_Range(state_change_time_min, state_change_time_max);
		param->is_attack_mode = false;
	}

	if (attack_mode_old != param->is_attack_mode)
	{
		attack_mode_old = param->is_attack_mode;
		if (param->is_attack_mode)
		{
			param->move_state = Move_State::Forward;
			anim->Set_Bool("Move", true);
			anim->Set_Int("Move_State", static_cast<int>(param->move_state));
		}
	}

	if (param->is_attack_mode)
	{
		const Vector3 self_pos = transform->Get_Position();
		Vector3 target_pos = target->Get_Position();
		target_pos.y = self_pos.y;
		if (Vector3::DistanceSquared(target_pos, self_pos) < powf(attack_distance, 2))
		{
			param->is_attack_mode = false;
			anim->Set_Trigger("Attack");
			anim->Set_Bool("Move", false);
		}
	}
	else
	{
		if (wait_state == Wait_State::Chase)
		{
			anim->Set_Bool("Move", true);
			anim->Set_Int("Move_State", static_cast<int>(param->move_state));
			const Vector3 self_pos = transform->Get_Position();
			Vector3 target_pos = target->Get_Position();
			target_pos.y = self_pos.y;
			if (Vector3::DistanceSquared(target_pos, self_pos) < powf(wait_distance, 2))
			{
				Change_Wait_State();
			}
		}
		else
		{
			wait_state_timer -= Time::delta_time;
			if (wait_state_timer <= 0)
			{
				wait_state_timer = Mathf::Random_Range(state_change_time_min, state_change_time_max);
				Change_Wait_State();
			}
		}
	}

	if (const bool attack_preliminary = anim->Get_Bool("Attack_Preliminary"); param->is_attack_preliminary != attack_preliminary)
	{
		param->is_attack_preliminary = attack_preliminary;
		if (param->is_attack_preliminary)
		{
			enemy_manager.lock()->Add_Attacking_List(parameter);
			flash_particle.lock()->Set_Active(true);
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

	if (anim->Get_Bool("Explosion"))
	{
		anim->Set_Bool("Explosion", false);
		if (param->hp <= 0)
		{
			pool.lock()->Instance_In_Pool("Explosion_01", spine_transform.lock()->Get_Position(), transform->Get_Rotation());
			param->living = false;
			gameobject->Set_Active(false);
		}
	}

	param->is_super_armor = anim->Get_Bool("Super_Armor");
}

void Enemy_Normal_01_State_Setter::Change_Wait_State()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	if (Mathf::Probability(probability_wait))
	{
		wait_state = Wait_State::Wait;
		anim->Set_Bool("Move", false);
	}
	else
	{
		if (Mathf::Probability(probability_chase))
		{
			wait_state = Wait_State::Chase;
			param->move_state = Move_State::Forward;
			wait_distance = Mathf::Random_Range(wait_distance_min, wait_distance_max);
		}
		else
		{
			if (Mathf::Probability(50))
			{
				wait_state = Wait_State::Right;
				param->move_state = Move_State::Right;
			}
			else
			{
				wait_state = Wait_State::Left;
				param->move_state = Move_State::Left;
			}
		}
		anim->Set_Bool("Move", true);
		anim->Set_Int("Move_State", static_cast<int>(param->move_state));
	}
}

bool Enemy_Normal_01_State_Setter::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Normal_01_State_Setter", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_DragFloat(u8"攻撃判定距離", "##Attack_Distance", &attack_distance, window_center);

		{
			ImGui::PushID("center_bone");
			ImGui::Text(u8"中心ボーン");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);

			string label_bone = u8"未設定 (ここにドラッグ)";
			if (const auto& p = spine_transform.lock())
			{
				label_bone = p->gameobject->name;
			}
			ImGui::InputText("##Item", &label_bone, ImGuiInputTextFlags_ReadOnly);

			if (ImGui::BeginDragDropTarget())
			{
				if (const auto& drag = Engine::editor->Get_Drag_Object())
				{
					spine_transform = drag->transform;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
		}

		{
			ImGui::PushID("flash_pos");
			ImGui::Text(u8"攻撃エフェクト座標");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);

			string label_flush = u8"未設定 (ここにドラッグ)";
			if (const auto& p = flash_particle.lock())
			{
				label_flush = p->name;
			}
			ImGui::InputText("##Item", &label_flush, ImGuiInputTextFlags_ReadOnly);

			if (ImGui::BeginDragDropTarget())
			{
				if (const auto& drag = Engine::editor->Get_Drag_Object())
				{
					flash_particle = drag;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
		}

		ImGui::LeftText_DragFloat(u8"最小行動時間", "##state_change_time_min", &state_change_time_min, window_center);
		ImGui::LeftText_DragFloat(u8"最大行動時間", "##state_change_time_max", &state_change_time_max, window_center);
		ImGui::LeftText_DragFloat(u8"待機確率", "##probability_wait", &probability_wait, window_center);
		ImGui::LeftText_DragFloat(u8"追跡確率", "##probability_chase", &probability_chase, window_center);
		ImGui::LeftText_DragFloat(u8"最小追跡終了距離", "##wait_distance_min", &wait_distance_min, window_center);
		ImGui::LeftText_DragFloat(u8"最大追跡終了距離", "##wait_distance_max", &wait_distance_max, window_center);
	}
	return true;
}