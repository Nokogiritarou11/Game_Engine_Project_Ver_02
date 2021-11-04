#include "Player_Move.h"
#include "Character_Parameter.h"
#include "Enemy_Manager.h"
#include "Player_Input.h"

using namespace std;
using namespace BeastEngine;

void Player_Move::Awake()
{
	p_input = Get_Component<Player_Input>();
	rigidbody = Get_Component<Capsule_Collider>()->rigidbody;
	animator = Get_Component<Animator>();
	parameter = Get_Component<Character_Parameter>();
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
}

void Player_Move::Move_Normal()
{
	const auto& rb = rigidbody.lock();
	Vector3 speed;

	if (const Vector3 move_forward = p_input.lock()->input_direction; move_forward != Vector3::Zero)
	{
		//滑らかに方向転換
		transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Local_Rotation(), transform->Look_At(transform->Get_Position() + move_forward), turn_speed * Time::delta_time));

		speed = transform->Get_Forward() * run_speed * Time::delta_time;
		speed.y = y_axis_velocity;
	}
	else
	{
		speed = Vector3(0, y_axis_velocity, 0);
	}
	rb->Set_Velocity(speed);
}

void Player_Move::Move_Attack()
{
	const auto& anim = animator.lock();
	const auto& input = p_input.lock();
	const auto& param = parameter.lock();

	move_speed = anim->Get_Float("Move_Speed");

	if (anim->Get_Bool("Turn_To_Enemy"))
	{
		anim->Set_Bool("Turn_To_Enemy", false);

		if (input->input_direction == Vector3::Zero)
		{
			Vector3 pos = param->target.lock()->transform->Get_Position();
			pos.y = transform->Get_Position().y;
			transform->Set_Local_Rotation(transform->Look_At(pos));
		}
		else
		{
			if (const auto& enemy_list = enemy_manager.lock()->enemy_list; !enemy_list.empty())
			{
				Vector3 nearest_position;
				float nearest_angle = FLT_MAX;
				for (const auto& enemy : enemy_list)
				{
					const Vector3 pos = enemy.lock()->transform->Get_Position();
					if (const float angle = input->Get_Input_To_Target_Angle(pos); angle < nearest_angle)
					{
						nearest_angle = angle;
						nearest_position = pos;
					}
				}

				nearest_position.y = transform->Get_Position().y;
				transform->Set_Local_Rotation(transform->Look_At(nearest_position));
			}
		}
	}

	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Player_Move::Move_Dodge()
{

}

void Player_Move::Move_Damage()
{
	const auto& anim = animator.lock();
	move_speed = anim->Get_Float("Move_Speed");

	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Player_Move::Move_Guard()
{
	const auto& anim = animator.lock();
	move_speed = anim->Get_Float("Move_Speed");

	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Player_Move::Move_Update()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

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
			y_axis_velocity -= down_power * Time::delta_time;
		}

		if (is_add_down)
		{
			y_axis_velocity -= down_power * 10 * Time::delta_time;
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

float Player_Move::Get_Forward_To_Target_Angle(Vector3 target_position) const
{
	const Vector3 player_pos = transform->Get_Position();
	target_position.y = player_pos.y;
	const Vector3 dir = transform->Get_Forward();
	Vector3 target_vector = target_position - (player_pos - dir * 1.5f);
	target_vector.Normalize();
	return acosf(dir.Dot(target_vector)) * 180.0f / static_cast<float>(M_PI);
}

bool Player_Move::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Player_Move", open)) return false;

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