#include "Player_Move.h"
#include "Character_Parameter.h"
#include "Enemy_Manager.h"

using namespace std;
using namespace BeastEngine;

void Player_Move::Awake()
{
	camera_transform = GameObject::Find_With_Tag("main_camera").lock()->transform;
	rigidbody = Get_Component<Capsule_Collider>()->rigidbody;
	animator = Get_Component<Animator>();
	parameter = Get_Component<Character_Parameter>();
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
}

void Player_Move::Move_Normal()
{
	Check_Move_Direction();

	const auto& rb = rigidbody.lock();
	Vector3 speed;

	if (move_forward != Vector3::Zero)
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
	Check_Move_Direction();
	const auto& anim = animator.lock();

	if (anim->Get_Bool("Turn_To_Enemy"))
	{
		anim->Set_Bool("Turn_To_Enemy", false);

		const auto& enemy_list = enemy_manager.lock()->enemy_list;
		if (!enemy_list.empty())
		{
			Vector3 nearest_position;
			float nearest_distance = FLT_MAX;
			for (const auto& enemy : enemy_list)
			{
				const Vector3 pos = enemy.lock()->transform->Get_Position();
				if (const float dis = Vector3::DistanceSquared(pos, transform->Get_Position()); dis < nearest_distance)
				{
					nearest_distance = dis;
					nearest_position = pos;
				}
			}
			nearest_position.y = transform->Get_Position().y;
			transform->Set_Local_Rotation(transform->Look_At(nearest_position));
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

}

void Player_Move::Move_Guard()
{

}

void Player_Move::Aerial_Update()
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

void Player_Move::Check_Move_Direction()
{
	const auto& camera_trans = camera_transform.lock();

	// カメラの方向から、X-Z平面の単位ベクトルを取得
	camera_forward = camera_trans->Get_Forward();
	camera_forward.y = 0;
	camera_forward.Normalize();

	// 方向キーの入力値とカメラの向きから、移動方向を決定
	const Vector2 axis = Input::Get_Pad_Axis_Left();
	(camera_forward * axis.y - camera_trans->Get_Right() * axis.x).Normalize(move_forward);

	const auto& anim = animator.lock();
	if (move_forward == Vector3::Zero)
	{
		anim->Set_Bool("Move", false);
	}
	else
	{
		anim->Set_Bool("Move", true);
	}
	move_speed = anim->Get_Float("Move_Speed");
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