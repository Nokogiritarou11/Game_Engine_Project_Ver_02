#include "Enemy_Normal_01_Move.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Normal_01_Move::Awake()
{
	rigidbody = Get_Component<Capsule_Collider>()->rigidbody;
	animator = Get_Component<Animator>();
	parameter = Get_Component<Character_Parameter>();
}

void Enemy_Normal_01_Move::Move_Normal()
{
	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Attack()
{

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

}

void Enemy_Normal_01_Move::Aerial_Update()
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
			const auto& rigid = rigidbody.lock();
			if (is_stay_air)
			{
				rigid->Set_Velocity(Vector3::Zero);
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
}

bool Enemy_Normal_01_Move::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Normal_01_Move", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"移動速度", "##Run_Speed", &run_speed, window_center);
		ImGui::LeftText_DragFloat(u8"回転速度", "##Turn_Speed", &turn_speed, window_center);
		ImGui::LeftText_DragFloat(u8"ジャンプの強さ", "##Jump_Power", &jump_power, window_center);
		ImGui::LeftText_DragFloat(u8"落下速度", "##Down_Power", &down_power, window_center);
	}
	return true;
}