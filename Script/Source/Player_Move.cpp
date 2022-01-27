#include "Player_Move.h"
#include "Player_Parameter.h"
#include "Enemy_Manager.h"
#include "Player_Input.h"

using namespace std;
using namespace BeastEngine;

void Player_Move::Awake()
{
	//メンバポインタの取得
	p_input = Get_Component<Player_Input>();
	collider = Get_Component<Capsule_Collider>();
	rigidbody = collider.lock()->rigidbody;
	animator = Get_Component<Animator>();
	parameter = Get_Component<Player_Parameter>();
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
}

void Player_Move::Move_Normal()
{
	const auto& rb = rigidbody.lock();
	Vector3 speed;

	if (const Vector3 move_forward = p_input.lock()->input_direction; move_forward == Vector3::Zero)
	{
		//入力がない場合は重力処理のみ
		speed = Vector3(0, y_axis_velocity, 0);
	}
	else
	{
		//入力方向へ滑らかに方向転換
		transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Local_Rotation(), transform->Look_At(transform->Get_Position() + move_forward), turn_speed * Time::delta_time));

		speed = transform->Get_Forward() * run_speed * Time::delta_time;
		//重力処理
		speed.y = y_axis_velocity;
	}
	rb->Set_Velocity(speed);
}

void Player_Move::Move_Attack()
{
	const auto& anim = animator.lock();
	const auto& input = p_input.lock();
	const auto& param = parameter.lock();

	//移動速度をアニメーションから取得する
	move_speed = anim->Get_Float("Move_Speed");

	//攻撃対象への向き直り
	if (anim->Get_Bool("Turn_To_Enemy"))
	{
		anim->Set_Bool("Turn_To_Enemy", false);

		if (input->input_direction == Vector3::Zero)
		{
			//入力がない場合はロックオン対象へ向く
			if (const auto& target = param->target.lock())
			{
				Vector3 pos = target->transform->Get_Position();
				pos.y = transform->Get_Position().y;
				transform->Set_Local_Rotation(transform->Look_At(pos));
			}
		}
		else
		{
			//入力がある場合は入力方向に一番近い敵の方へ向く
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
	//重力処理
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Player_Move::Move_Damage()
{
	const auto& anim = animator.lock();
	move_speed = anim->Get_Float("Move_Speed");

	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	//重力処理
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Player_Move::Move_Guard()
{
	const auto& anim = animator.lock();
	move_speed = anim->Get_Float("Move_Speed");

	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	//重力処理
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Player_Move::Move_Update()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();
	const auto& rb = rigidbody.lock();

	rb->Set_Angular_Velocity(Vector3::Zero);

	if (param->is_ground)
	{
		//ジャンプ判定
		if (anim->Get_Bool("Add_Jump_Force"))
		{
			y_axis_velocity = jump_power;
			anim->Set_Bool("Add_Jump_Force", false);
		}
	}
	else
	{
		//空中停止
		if (const bool stay_air = anim->Get_Bool("Stay_Air"); is_stay_air != stay_air)
		{
			is_stay_air = stay_air;
			const auto& rigid = rigidbody.lock();
			if (is_stay_air)
			{
				//重力を0に
				rigid->Set_Velocity(Vector3::Zero);
				y_axis_velocity = 0;
			}
		}

		is_add_down = anim->Get_Bool("Add_Down_Force");
		//重力を加算
		if (!is_stay_air && !is_add_down)
		{
			y_axis_velocity -= down_power * Time::delta_time;
		}

		//下方向へ速度を与える
		if (is_add_down)
		{
			y_axis_velocity -= down_power * 10 * Time::delta_time;
		}
	}

	if (anim->Get_Bool("No_Collision") != no_collision_old)
	{
		no_collision_old = !no_collision_old;
		if (no_collision_old)
		{
			collider.lock()->Set_Layer(5);
		}
		else
		{
			collider.lock()->Set_Layer(1);
		}
	}
}

float Player_Move::Get_Forward_To_Target_Angle(Vector3 target_position) const
{
	//引数の座標とプレイヤーの正面ベクトルの角度さを求める
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