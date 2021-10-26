#include "Enemy_Normal_01_Move.h"
#include "Character_Parameter.h"
#include "Enemy_Manager.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Normal_01_Move::Awake()
{
	rigidbody = Get_Component<Capsule_Collider>()->rigidbody;
	animator = Get_Component<Animator>();
	parameter = Get_Component<Character_Parameter>();
	target_transform = GameObject::Find_With_Tag("player").lock()->transform;
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
}

void Enemy_Normal_01_Move::Move_Normal()
{
	const auto& target = target_transform.lock();
	Vector3 target_position = target->Get_Position();
	target_position.y = transform->Get_Position().y;
	//ŠŠ‚ç‚©‚É•ûŒü“]Š·
	transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Local_Rotation(), transform->Look_At(target_position), turn_speed * Time::delta_time));

	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * run_speed * Time::delta_time;
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Attack()
{
	if(is_attack_preliminary)
	{
		const auto& target = target_transform.lock();
		Vector3 target_position = target->Get_Position();
		target_position.y = transform->Get_Position().y;
		//ŠŠ‚ç‚©‚É•ûŒü“]Š·
		transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Local_Rotation(), transform->Look_At(target_position), turn_speed * Time::delta_time));
	}
	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Dodge()
{

}

void Enemy_Normal_01_Move::Move_Damage()
{
	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Guard()
{
	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Update()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	move_speed = anim->Get_Float("Move_Speed");

	if (!param->is_ground)
	{
		is_add_down = anim->Get_Bool("Add_Down_Force");

		if (const bool stay_air = anim->Get_Bool("Stay_Air"); is_stay_air != stay_air)
		{
			is_stay_air = stay_air;
			if (is_stay_air)
			{
				y_axis_velocity = 0;
			}
		}

		if (!is_stay_air && !is_add_down)
		{
			y_axis_velocity -= down_power;
		}

		if (is_add_down)
		{
			y_axis_velocity -= down_power * 10;
		}
	}
	else
	{
		if (anim->Get_Bool("Add_Jump_Force"))
		{
			y_axis_velocity = jump_power;
			anim->Set_Bool("Add_Jump_Force", false);
		}
	}

	if (const bool attack_preliminary = anim->Get_Bool("Attack_Preliminary"); is_attack_preliminary != attack_preliminary)
	{
		is_attack_preliminary = attack_preliminary;
		if (is_attack_preliminary)
		{
			enemy_manager.lock()->Add_Attacking_List(gameobject);
		}
		else
		{
			enemy_manager.lock()->Remove_Attacking_List(gameobject);
		}
	}
}

bool Enemy_Normal_01_Move::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Normal_01_Move", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"ˆÚ“®‘¬“x", "##Run_Speed", &run_speed, window_center);
		ImGui::LeftText_DragFloat(u8"‰ñ“]‘¬“x", "##Turn_Speed", &turn_speed, window_center);
		ImGui::LeftText_DragFloat(u8"ƒWƒƒƒ“ƒv‚Ì‹­‚³", "##Jump_Power", &jump_power, window_center);
		ImGui::LeftText_DragFloat(u8"—Ž‰º‘¬“x", "##Down_Power", &down_power, window_center);
	}
	return true;
}