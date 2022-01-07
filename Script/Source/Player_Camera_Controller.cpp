#include "Player_Camera_Controller.h"
#include "Player_Parameter.h"
#include "Enemy_Manager.h"
#include "Interface_Cut_Scene.h"

using namespace std;
using namespace BeastEngine;

void Player_Camera_Controller::Awake()
{
	//メンバポインタの取得
	const auto& obj = GameObject::Find_With_Tag("player").lock();
	player_transform = obj->transform;
	parameter = obj->Get_Component<Player_Parameter>();
	camera_transform = GameObject::Find_With_Tag("main_camera").lock()->transform;
	camera = camera_transform.lock()->Get_Component<Camera>();
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();

	//初期位置を保存する
	final_position = camera_transform.lock()->Get_Local_Position();

	//使用するカットシーンを登録する
	cut_scene.resize(3);
	cut_scene[0] = transform->Find("Cut_Scene_Smash_01").lock()->Get_Component<Interface_Cut_Scene>();
	cut_scene[2] = transform->Find("Cut_Scene_Smash_03").lock()->Get_Component<Interface_Cut_Scene>();
}

void Player_Camera_Controller::LateUpdate()
{
	const auto& e_manager = enemy_manager.lock();
	const auto& param = parameter.lock();

	if (is_playing_cut_scene)
	{
		//カットシーン再生中の場合
		//カットシーンの再生が終了するまで待機
		const auto& cut = playing_cut_scene.lock();
		if (cut->is_end_play)
		{
			//再生が終了したのでカメラの位置を登録する
			final_position = camera_transform.lock()->Get_Local_Position();
			is_playing_cut_scene = false;
		}
		else
		{
			final_position = cut->Play_Cut_Scene();
		}
	}
	else
	{
		if (e_manager->enemy_list.empty())
		{
			//敵が居ない場合
			Update_Free_Look();
		}
		else
		{
			if (param->camera_locking && !param->target.expired())
			{
				//ロックオン中
				Update_Lock_On();
			}
			else
			{
				//非ロックオンかつ戦闘中
				Update_Battle();
			}
		}
	}

	//カメラシェイク回数が残っている場合
	if (shake_count > 0)
	{
		//タイマーの更新
		shake_timer += Time::delta_time;

		//60fps基準でカメラシェイクを行う
		if (constexpr float frame_time = 1.0f / 60.0f; shake_timer <= frame_time)
		{
			//カメラシェイク処理
			const auto& camera_trans = camera_transform.lock();
			const Vector3 pos = final_position;
			const float x = pos.x + Mathf::Random_Range(-1.0f, 1.0f) * shake_power;
			const float y = pos.y + Mathf::Random_Range(-1.0f, 1.0f) * shake_power;
			camera_trans->Set_Local_Position(x, y, pos.z);
			shake_timer = 0;
			//残りのシェイク回数を減らす
			--shake_count;
		}
	}
}

void Player_Camera_Controller::Update_Free_Look()
{
	const auto& p_trans = player_transform.lock();
	const auto& param = parameter.lock();

	//親
	{
		//移動入力を取得
		const Vector2 left_axis = Input::Get_Pad_Axis_Left();

		const Vector3 now_pos = transform->Get_Position();
		Vector3 follow_pos = p_trans->Get_Position();
		follow_pos.y += 1;

		//プレイヤーへの追従処理
		//移動入力によって追従速度を変える
		follow_pos.x = Mathf::Lerp(now_pos.x, follow_pos.x, follow_speed * (1 + abs(left_axis.y) * 0.75f) * Time::delta_time);
		follow_pos.y = Mathf::Lerp(now_pos.y, follow_pos.y, follow_speed * 0.75f * Time::delta_time);
		follow_pos.z = Mathf::Lerp(now_pos.z, follow_pos.z, follow_speed * (1 + abs(left_axis.y) * 0.75f) * Time::delta_time);

		transform->Set_Local_Position(follow_pos);

		//カメラの回転処理
		const Vector2 axis = Input::Get_Pad_Axis_Right();
		Vector3 angle = { -axis.y * rotate_speed * Time::delta_time, -axis.x * rotate_speed * Time::delta_time, 0 };

		//移動方向によってカメラを自動回転させる
		if (param->moving)
		{
			angle.y -= left_axis.x * rotate_speed * 0.25f * Time::delta_time;
		}

		const Vector3 rot = transform->Get_Euler_Angles() + angle;

		//最大角度にクランプ
		const float angle_x = 180.0f <= rot.x ? rot.x - 360 : rot.x;
		transform->Set_Local_Euler_Angles(Mathf::Clamp(angle_x, angle_limit_down, angle_limit_up), rot.y, rot.z);
	}

	//カメラ本体
	{
		//指定位置へ滑らかに移動させる
		const auto& camera_trans = camera_transform.lock();
		camera_trans->Set_Local_Position(Vector3::Lerp(final_position, default_position, Time::delta_time));
		camera_trans->Set_Local_Euler_Angles(Vector3::Lerp(camera_trans->Get_Local_Euler_Angles(), default_rotation, Time::delta_time));
		const Vector3 r = camera_trans->Get_Euler_Angles();
		camera_trans->Set_Euler_Angles(r.x, r.y, 0);

		final_position = camera_trans->Get_Local_Position();
	}
}

void Player_Camera_Controller::Update_Battle()
{
	const auto& param = parameter.lock();
	const auto& e_manager = enemy_manager.lock();
	const auto& p_trans = player_transform.lock();
	const auto& camera_trans = camera_transform.lock();
	const auto& target = e_manager->last_attack_target.lock();

	//最終攻撃対象をロックオン対象に設定
	param->target = target;

	const int enemy_count = e_manager->enemy_list.size();
	const Vector2 target_view_pos = camera.lock()->World_To_Viewport_Point(target->transform->Get_Position());

	//親
	{
		//プレイヤーへの追従処理
		const Vector3 now_pos = transform->Get_Position();
		Vector3 follow_pos = p_trans->Get_Position();
		follow_pos.y += 1;

		follow_pos.x = Mathf::Lerp(now_pos.x, follow_pos.x, follow_speed * Time::delta_time);
		follow_pos.y = Mathf::Lerp(now_pos.y, follow_pos.y, follow_speed * 0.75f * Time::delta_time);
		follow_pos.z = Mathf::Lerp(now_pos.z, follow_pos.z, follow_speed * Time::delta_time);

		transform->Set_Local_Position(follow_pos);

		//カメラの回転処理
		const Vector2 axis = Input::Get_Pad_Axis_Right();
		Vector3 angle = { -axis.y * rotate_speed * Time::delta_time, -axis.x * rotate_speed * Time::delta_time, 0 };

		if (param->moving || param->attacking)
		{
			// カメラの方向から、X-Z平面の単位ベクトルを取得
			Vector3 camera_direction = camera_trans->Get_Forward();
			camera_direction.y = 0;
			camera_direction.Normalize();

			const Vector3 axis_camera = camera_direction.Cross(p_trans->Get_Forward());
			const float angle_camera = acosf(camera_direction.Dot(p_trans->Get_Forward())) * 180.0f / static_cast<float>(M_PI) * (axis_camera.y < 0 ? -1 : 1) / 180.0f;
			if (angle_camera >= 0.4f)
			{
				angle.y += (angle_camera - 0.4f) * rotate_speed * 0.5f * Time::delta_time;
			}
			else if (angle_camera <= -0.4f)
			{
				angle.y += (angle_camera + 0.4f) * rotate_speed * 0.5f * Time::delta_time;
			}
		}
		const Vector3 rot = transform->Get_Euler_Angles() + angle;

		//最大角度にクランプ
		const float angle_x = 180.0f <= rot.x ? rot.x - 360 : rot.x;
		transform->Set_Local_Euler_Angles(Mathf::Clamp(angle_x, angle_limit_down, angle_limit_up), rot.y, rot.z);
	}

	//カメラ本体
	{
		Vector3 pos = battle_position;
		Vector3 rot = battle_rotation;

		if (is_target_right)
		{
			//中止対象が画面右側にいる場合
			//中止対象が左に行ったら切り替え
			if (target_view_pos.x < 0.425f)
			{
				is_target_right = false;
			}
		}
		else
		{
			//中止対象が画面左側にいる場合
			//中止対象が右に行ったら切り替え
			if (target_view_pos.x > 0.575f)
			{
				is_target_right = true;
			}
			//姿勢を左右反転
			pos.x *= -1;
			rot.y *= -1;
		}

		//プレイヤーが空中にいる場合は少しカメラを引く
		if (!param->is_ground)
		{
			pos.z -= 0.75f;
		}

		//敵の数によってカメラを引く
		pos.z -= 0.5f * (Mathf::Clamp(static_cast<float>(enemy_count), 0, 4) - 1);

		camera_trans->Set_Local_Position(Vector3::Lerp(final_position, pos, Time::delta_time * 2.0f));

		camera_trans->Set_Local_Euler_Angles(Vector3::Lerp(camera_trans->Get_Local_Euler_Angles(), rot, Time::delta_time * 2.0f));
		const Vector3 r = camera_trans->Get_Euler_Angles();
		camera_trans->Set_Euler_Angles(r.x, r.y, 0);

		final_position = camera_trans->Get_Local_Position();
	}
}

void Player_Camera_Controller::Update_Lock_On()
{
	const auto& param = parameter.lock();
	const auto& p_trans = player_transform.lock();
	const auto& camera_trans = camera_transform.lock();
	const auto& target = param->target.lock()->lock_on_target.lock();

	const Vector2 target_view_pos = camera.lock()->World_To_Viewport_Point(target->Get_Position());

	//親
	{
		//プレイヤーへの追従処理
		const Vector3 now_pos = transform->Get_Position();
		Vector3 follow_pos = p_trans->Get_Position();
		follow_pos.y += 1;

		follow_pos.x = Mathf::Lerp(now_pos.x, follow_pos.x, follow_speed * Time::delta_time);
		follow_pos.y = Mathf::Lerp(now_pos.y, follow_pos.y, follow_speed * 0.75f * Time::delta_time);
		follow_pos.z = Mathf::Lerp(now_pos.z, follow_pos.z, follow_speed * Time::delta_time);

		transform->Set_Local_Position(follow_pos);

		//カメラの回転処理
		transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Rotation(), transform->Look_At(target->Get_Position()), lock_on_targeting_speed * Time::delta_time));
	}

	//カメラ本体
	{
		Vector3 pos = lock_position;
		Vector3 rot = lock_rotation;

		if (is_target_right)
		{
			//中止対象が画面右側にいる場合
			//中止対象が左に行ったら切り替え
			if (target_view_pos.x < 0.35f)
			{
				is_target_right = false;
			}
		}
		else
		{
			//中止対象が画面左側にいる場合
			//中止対象が右に行ったら切り替え
			if (target_view_pos.x > 0.65f)
			{
				is_target_right = true;
			}
			//姿勢を左右反転
			pos.x *= -1;
			rot.y *= -1;
		}

		//プレイヤーが空中にいる場合は少しカメラを引く
		if (!param->is_ground)
		{
			pos.z -= 0.75f;
		}

		camera_trans->Set_Local_Position(Vector3::Lerp(final_position, pos, Time::delta_time * 2.0f));

		camera_trans->Set_Local_Euler_Angles(Vector3::Lerp(camera_trans->Get_Local_Euler_Angles(), rot, Time::delta_time * 2.0f));
		const Vector3 r = camera_trans->Get_Euler_Angles();
		camera_trans->Set_Euler_Angles(r.x, r.y, 0);

		final_position = camera_trans->Get_Local_Position();
	}
}

void Player_Camera_Controller::Shake_Camera(const int& count, const float& power)
{
	if (count > 0)
	{
		//カメラシェイク回数を追加する
		shake_count = count;
		shake_power = power;
		shake_timer = 0;
	}
}

void Player_Camera_Controller::Play_Cut_Scene(const int& index)
{
	if (index < static_cast<int>(cut_scene.size()))
	{
		//カットシーンの再生
		playing_cut_scene = cut_scene[index];
		playing_cut_scene.lock()->is_end_play = false;
		is_playing_cut_scene = true;
	}
}


bool Player_Camera_Controller::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Player_Camera_Controller", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"上方向角度最大", "##Angle_Limit_Up", &angle_limit_up, window_center);
		ImGui::LeftText_DragFloat(u8"下方向角度最大", "##Angle_Limit_Down", &angle_limit_down, window_center);
		ImGui::LeftText_DragFloat(u8"回転速度", "##Rotate_Speed", &rotate_speed, window_center);
		ImGui::LeftText_DragFloat(u8"追従速度", "##Follow_Speed", &follow_speed, window_center);
		ImGui::LeftText_DragFloat(u8"ロック時旋回追従速度", "##Lock_On_Targeting_Speed", &lock_on_targeting_speed, window_center);

		if (ImGui::TreeNode(u8"平常時カメラ姿勢"))
		{
			ImGui::LeftText_DragFloat3(u8"位置", "##Default_Position", default_position, window_center);
			ImGui::LeftText_DragFloat3(u8"向き", "##Default_Rotation", default_rotation, window_center);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(u8"戦闘時カメラ姿勢"))
		{
			ImGui::LeftText_DragFloat3(u8"位置", "##Battle_Position", battle_position, window_center);
			ImGui::LeftText_DragFloat3(u8"向き", "##Battle_Rotation", battle_rotation, window_center);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(u8"ロックオン時カメラ姿勢"))
		{
			ImGui::LeftText_DragFloat3(u8"位置", "##Lock_Position", lock_position, window_center);
			ImGui::LeftText_DragFloat3(u8"向き", "##Lock_Rotation", lock_rotation, window_center);
			ImGui::TreePop();
		}
	}
	return true;
}