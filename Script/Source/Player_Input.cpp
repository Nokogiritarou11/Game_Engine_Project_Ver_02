#include "Player_Input.h"
#include "Enemy_Manager.h"
#include "Player_Parameter.h"
#include "Player_Camera_Controller.h"

using namespace std;
using namespace BeastEngine;

void Player_Input::Awake()
{
	//メンバポインタの取得
	animator = Get_Component<Animator>();
	parameter = Get_Component<Player_Parameter>();
	camera_transform = GameObject::Find_With_Tag("main_camera").lock()->transform;
	camera_controller = camera_transform.lock()->Get_Parent().lock()->Get_Component<Player_Camera_Controller>();
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
}

void Player_Input::Set_State()
{
	const auto& anim = animator.lock();
	const auto& camera_trans = camera_transform.lock();
	const auto& param = parameter.lock();
	const auto& e_manager = enemy_manager.lock();

	// カメラの方向から、X-Z平面の単位ベクトルを取得
	Vector3 camera_direction = camera_trans->Get_Forward();
	camera_direction.y = 0;
	camera_direction.Normalize();

	// 方向キーの入力値とカメラの向きから、移動方向を決定
	const Vector2 axis = Input::Get_Pad_Axis_Left();
	(camera_direction * axis.y - camera_trans->Get_Left() * axis.x).Normalize(input_direction);

	//移動入力
	const bool stick_neutral = input_direction == Vector3::Zero;
	anim->Set_Bool("Move", !stick_neutral);

	//ガード用にスティックのニュートラルを確認してフラグをリセット
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

	//ロックオン
	if (Input::Get_Pad_Button_Down(Button_Code::Right_Shoulder))
	{
		if (!enemy_manager.lock()->enemy_list.empty())
		{
			param->camera_locking = !param->camera_locking;
		}
	}

	//スティックがニュートラル状態から倒された直後
	if (can_guard_stick)
	{
		//タイマーの更新
		guard_stick_timer -= Time::delta_time;
		if (guard_stick_timer <= 0) can_guard_stick = false;
	}
	//弱攻撃ボタンが押された直後
	if (can_guard_button)
	{
		//タイマーの更新
		guard_button_timer -= Time::delta_time;
		if (guard_button_timer <= 0) can_guard_button = false;
	}

	//ガード入力成立時
	if (can_guard_button && can_guard_stick && !param->guarding)
	{
		//攻撃中の敵が存在しているか
		if (const auto& attacking_list = e_manager->attacking_list; !attacking_list.empty())
		{
			for (const auto& enemy : attacking_list)
			{
				//スティックを倒した方向に攻撃中の敵がいるかを確認
				if (Vector3 target_pos = enemy.lock()->transform->Get_Position(); Get_Input_To_Target_Angle(target_pos) <= guard_degree)
				{
					//ガード成立
					anim->Set_Trigger("Guard");
					//攻撃相手の方を向く
					target_pos.y = transform->Get_Position().y;
					input_direction = Vector3::Zero;
					transform->Set_Local_Rotation(transform->Look_At(target_pos));
					break;
				}
			}
		}
		can_guard_button = false;
		can_guard_stick = false;
	}

	//特殊攻撃
	//スタン中の敵が存在する場合
	if (const auto& stunning_list = e_manager->stunning_list; !stunning_list.empty())
	{
		for (const auto& enemy : stunning_list)
		{
			//一定距離以内に存在するか確認
			if (const auto& enemy_lock = enemy.lock(); Vector3::DistanceSquared(transform->Get_Position(), enemy_lock->transform->Get_Position()) <= powf(2.5f, 2))
			{
				//待機状態でボタンの入力があったかをチェック
				if (Input::Get_Pad_Button_Down(Button_Code::B) && param->is_ground && !param->attacking && !param->damaging && !param->guarding && !param->is_invincible)
				{
					//お互いが向き直る
					Vector3 pos = transform->Get_Position();
					Vector3 enemy_pos = enemy_lock->transform->Get_Position();
					enemy_pos.y = pos.y;
					pos.y = enemy_pos.y;
					transform->Set_Local_Rotation(transform->Look_At(enemy_pos));
					enemy_lock->transform->Set_Local_Rotation(enemy_lock->transform->Look_At(pos));
					enemy_lock->transform->Set_Local_Position(pos + transform->Get_Forward() * 2.0f);

					//敵のアニメーションを再生し、死亡させる
					enemy_lock->Get_Component<Collider>()->Set_Layer(5);
					enemy_lock->Get_Component<Animator>()->Set_Trigger("Smash");
					enemy_manager.lock()->Enemy_Dead(false, enemy);

					//カットシーンを再生
					camera_controller.lock()->Play_Cut_Scene(static_cast<int>(enemy_lock->type));
					anim->Set_Int("Smash_Number", static_cast<int>(enemy_lock->type));
					input_direction = Vector3::Zero;
				}
				break;
			}
		}
	}
}

float Player_Input::Get_Input_To_Target_Angle(Vector3 target_position) const
{
	//引数の座標の入力方向に対しての角度差を算出する
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