#include "Player_Input.h"
#include "Enemy_Manager.h"

using namespace std;
using namespace BeastEngine;

void Player_Input::Awake()
{
	animator = Get_Component<Animator>();
	camera_transform = GameObject::Find_With_Tag("main_camera").lock()->transform;
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
}

void Player_Input::Set_State()
{
	const auto& anim = animator.lock();
	const auto& camera_trans = camera_transform.lock();

	// カメラの方向から、X-Z平面の単位ベクトルを取得
	Vector3 camera_direction = camera_trans->Get_Forward();
	camera_direction.y = 0;
	camera_direction.Normalize();

	// 方向キーの入力値とカメラの向きから、移動方向を決定
	const Vector2 axis = Input::Get_Pad_Axis_Left();
	(camera_direction * axis.y - camera_trans->Get_Right() * axis.x).Normalize(input_direction);

	const bool stick_neutral = input_direction == Vector3::Zero;
	anim->Set_Bool("Move", !stick_neutral);

	if (stick_neutral) is_stick_reset = true;
	if (is_stick_reset && !stick_neutral)
	{
		is_stick_reset = false;
		can_guard_stick = true;
		guard_stick_timer = guard_stick_time;
	}

	//弱攻撃
	if (Input::Get_Pad_Button_Down(Button_Code::X))
	{
		anim->Set_Int("Attack_State", 1);
		can_guard_button = true;
		guard_button_timer = guard_button_time;
	}

	//強攻撃
	if (Input::Get_Pad_Button_Down(Button_Code::Y))
	{
		anim->Set_Int("Attack_State", 2);
	}
	//ジャンプ
	if (Input::Get_Pad_Button_Down(Button_Code::A))
	{
		anim->Set_Trigger("Jump");
	}

	if (can_guard_stick)
	{
		guard_stick_timer -= Time::delta_time;
		if (guard_stick_timer <= 0) can_guard_stick = false;
	}
	if (can_guard_button)
	{
		guard_button_timer -= Time::delta_time;
		if (guard_button_timer <= 0) can_guard_button = false;
	}

	if (can_guard_button && can_guard_stick)
	{
		if (const auto& attacking_list = enemy_manager.lock()->attacking_list; !attacking_list.empty())
		{
			for (const auto& enemy : attacking_list)
			{
				if (Vector3 target_pos = enemy.lock()->transform->Get_Position(); Get_Input_To_Target_Angle(target_pos) <= guard_degree)
				{
					target_pos.y = transform->Get_Position().y;
					anim->Set_Trigger("Guard");
					input_direction = Vector3::Zero;
					transform->Set_Local_Rotation(transform->Look_At(target_pos));
					break;
				}
			}
		}
		can_guard_button = false;
		can_guard_stick = false;
	}
}

float Player_Input::Get_Input_To_Target_Angle(Vector3 target_position) const
{
	const Vector3 player_pos = transform->Get_Position();
	target_position.y = player_pos.y;
	Vector3 target_vector = target_position - (player_pos - input_direction * 1.5f);
	target_vector.Normalize();
	return acosf(input_direction.Dot(target_vector)) * 180.0f / static_cast<float>(M_PI);
}

bool Player_Input::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Player_Input", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_DragFloat(u8"弾き可能角度", "##Guard_Degree", &guard_degree, window_center);
		ImGui::LeftText_DragFloat(u8"ガード判定時間_スティック", "##Guard_Stick_Time", &guard_stick_time, window_center);
		ImGui::LeftText_DragFloat(u8"ガード判定時間_ボタン", "##Guard_Button_Time", &guard_button_time, window_center);
	}
	return true;
}