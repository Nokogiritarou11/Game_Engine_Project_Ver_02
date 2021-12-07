#include "Enemy_Boss_01_Move.h"
#include "Enemy_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Boss_01_Move::Awake()
{
	//メンバポインタの取得
	collider = Get_Component<Capsule_Collider>();
	rigidbody = collider.lock()->rigidbody;
	animator = Get_Component<Animator>();
	parameter = Get_Component<Enemy_Parameter>();
	target_transform = GameObject::Find_With_Tag("player").lock()->transform;
}

void Enemy_Boss_01_Move::Move_Normal()
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
			case Move_State::Back:
				speed = -transform->Get_Forward() * run_speed * Time::delta_time;
				break;
			default:
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

void Enemy_Boss_01_Move::Move_Attack()
{
	switch (parameter.lock()->attack_state)
	{
		case 0:
			Attack_Melee();
			break;
		case 1:
			Attack_Melee();
			break;
		case 2:
			Attack_Dash();
			break;
		case 3:
			Attack_Jump();
			break;
		default:
			break;
	}
}

void Enemy_Boss_01_Move::Move_Damage()
{
	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	//重力処理
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Boss_01_Move::Move_Guard()
{
	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	//重力処理
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Boss_01_Move::Move_Update()
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

void Enemy_Boss_01_Move::Attack_Melee() const
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

void Enemy_Boss_01_Move::Attack_Dash()
{
	const auto& anim = animator.lock();
	const auto& rb = rigidbody.lock();
	const auto& target = target_transform.lock();
	Vector3 speed = Vector3::Zero;

	if (const int state = anim->Get_Int("Attack_Dash_State"); state == 0)
	{
		Vector3 target_position = target->Get_Position();
		target_position.y = transform->Get_Position().y;
		//プレイヤー方向へ滑らかに方向転換
		transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Local_Rotation(), transform->Look_At(target_position), turn_speed * 2.0f * Time::delta_time));
	}
	else if (state == 1)
	{
		//衝突しないようにレイヤーを切り替え
		collider.lock()->Set_Layer(5);
		anim->Set_Int("Attack_Dash_State", 2);
	}
	else if (state == 2)
	{
		//攻撃距離に達するまで直進
		const Vector3 self_pos = transform->Get_Position();
		Vector3 target_pos = target->Get_Position();
		target_pos.y = self_pos.y;
		if (Vector3::DistanceSquared(target_pos, self_pos) < powf(3.5f, 2))
		{
			anim->Set_Int("Attack_Dash_State", 3);
		}
		//進行方向を保存
		dash_forward = transform->Get_Forward();
		speed = dash_forward * move_speed * Time::delta_time;
	}
	else if (state == 3)
	{
		Vector3 target_position = target->Get_Position();
		target_position.y = transform->Get_Position().y;
		//プレイヤー方向へ滑らかに方向転換
		transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Local_Rotation(), transform->Look_At(target_position), turn_speed * Time::delta_time));

		//進行方向は維持
		speed = dash_forward * move_speed * Time::delta_time;
	}
	else if (state == 4)
	{
		//レイヤーを戻す
		collider.lock()->Set_Layer(2);
		anim->Set_Int("Attack_Dash_State", 5);
	}

	//重力処理
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Boss_01_Move::Attack_Jump()
{
	const auto& anim = animator.lock();
	const auto& target = target_transform.lock();
	const auto& rb = rigidbody.lock();

	if (const int state = anim->Get_Int("Attack_Jump_State"); state == 0)
	{
		Vector3 target_position = target->Get_Position();
		target_position.y = transform->Get_Position().y;
		//プレイヤー方向へ滑らかに方向転換
		transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Local_Rotation(), transform->Look_At(target_position), turn_speed * 3.0f * Time::delta_time));
	}
	else if (state == 1)
	{
		//移動先を算出
		jump_start_position = transform->Get_Position();
		jump_attack_rate_timer = 0;
		anim->Set_Int("Attack_Jump_State", 2);
	}
	else if (state == 2)
	{
		constexpr float jump_all_time = 0.8f;
		constexpr float jump_up_time = 0.3f;
		constexpr float jump_down_time = 0.25f;
		constexpr float jump_height = 3.0f;
		//アニメーションに合わせて指定時間で移動
		jump_attack_rate_timer += Time::delta_time;
		const float rate = jump_attack_rate_timer / jump_all_time;
		Vector3 target_pos = target->Get_Position();
		target_pos.y = jump_start_position.y;
		Vector3 target_dir = target_pos - jump_start_position;
		target_dir.Normalize();
		Vector3 jump_end_position = Vector3::Lerp(jump_start_position, target_pos - target_dir * 2.0f, rate);

		float jump_y = 0;
		if (jump_attack_rate_timer <= jump_up_time)
		{
			jump_y = Mathf::Lerp(0, jump_height, jump_attack_rate_timer / jump_up_time);
		}
		else if (jump_attack_rate_timer <= jump_all_time - jump_down_time)
		{
			jump_y = jump_height;
		}
		else if(jump_attack_rate_timer <= jump_all_time)
		{
			jump_y = Mathf::Lerp(jump_height, 0, (jump_attack_rate_timer - (jump_all_time - jump_down_time)) / jump_down_time);
		}
		jump_end_position.y = jump_y;
		transform->Set_Local_Position(jump_end_position);
	}

	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	//重力処理
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

bool Enemy_Boss_01_Move::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Boss_01_Move", open)) return false;

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