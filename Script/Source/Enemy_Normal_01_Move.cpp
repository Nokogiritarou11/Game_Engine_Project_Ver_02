#include "Enemy_Normal_01_Move.h"
#include "Enemy_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Normal_01_Move::Awake()
{
	//メンバポインタの取得
	rigidbody = Get_Component<Capsule_Collider>()->rigidbody;
	animator = Get_Component<Animator>();
	parameter = Get_Component<Enemy_Parameter>();
	target_transform = GameObject::Find_With_Tag("player").lock()->transform;
}

void Enemy_Normal_01_Move::Move_Normal()
{
	const auto& target = target_transform.lock();
	Vector3 target_position = target->Get_Position();
	target_position.y = transform->Get_Position().y;
	//プレイヤー方向へ滑らかに方向転換
	transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Local_Rotation(), transform->Look_At(target_position), turn_speed * Time::delta_time));

	const auto& rb = rigidbody.lock();
	Vector3 speed;
	//移動状態時
	if (const auto& param = parameter.lock(); param->moving)
	{
		//移動方向によって速度方向も変化
		switch (param->move_state)
		{
			case Move_State::Forward:
				speed = transform->Get_Forward() * run_speed * Time::delta_time;
				break;
			case Move_State::Right:
				speed = -transform->Get_Left() * run_speed * 0.25f * Time::delta_time;
				break;
			case Move_State::Left:
				speed = transform->Get_Left() * run_speed * 0.25f * Time::delta_time;
				break;
		}
		//重力処理
		speed.y = y_axis_velocity;
	}
	else
	{
		//重力処理
		speed = Vector3(0, y_axis_velocity, 0);
	}
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Attack()
{
	//攻撃前モーション中の場合
	if (const auto& param = parameter.lock(); param->is_attack_preliminary)
	{
		const auto& target = target_transform.lock();
		Vector3 target_position = target->Get_Position();
		target_position.y = transform->Get_Position().y;
		//プレイヤー方向へ滑らかに方向転換
		transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Local_Rotation(), transform->Look_At(target_position), turn_speed * Time::delta_time));
	}
	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	//重力処理
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Damage()
{
	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	//重力処理
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Guard()
{
	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	//重力処理
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Update()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	//アニメーションから移動速度を得る
	move_speed = anim->Get_Float("Move_Speed");

	if (param->is_ground)
	{
		//地上にいる場合
		//Animatorからジャンプするかを判断
		if (anim->Get_Bool("Add_Jump_Force"))
		{
			y_axis_velocity = jump_power;
			anim->Set_Bool("Add_Jump_Force", false);
		}
	}
	else
	{
		//空中にいる場合
		//空中攻撃などで空中に留まるかをAnimatorから判断
		if (const bool stay_air = anim->Get_Bool("Stay_Air"); is_stay_air != stay_air)
		{
			//重力を切る
			is_stay_air = stay_air;
			if (is_stay_air)
			{
				y_axis_velocity = 0;
			}
		}

		//重力を加算する
		if (!is_stay_air)
		{
			y_axis_velocity -= down_power * Time::delta_time;
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