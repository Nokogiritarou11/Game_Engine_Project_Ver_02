#include "Player_Camera_Controller.h"
#include "Character_Parameter.h"
#include  "Enemy_Manager.h"

using namespace std;
using namespace BeastEngine;

void Player_Camera_Controller::Awake()
{
	const auto& obj = GameObject::Find_With_Tag("player").lock();
	player_transform = obj->transform;
	parameter = obj->Get_Component<Character_Parameter>();
	camera_transform = GameObject::Find_With_Tag("main_camera").lock()->transform;
	camera = camera_transform.lock()->Get_Component<Camera>();
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
}

void Player_Camera_Controller::LateUpdate()
{
	const auto& e_manager = enemy_manager.lock();
	const auto& param = parameter.lock();

	if (e_manager->enemy_list.empty())
	{
		Update_Free_Look();
	}
	else
	{
		if (param->camera_locking)
		{
			Update_Lock_On();
		}
		else
		{
			Update_Battle();
		}
	}
}

void Player_Camera_Controller::Update_Free_Look() const
{
	const auto& p_trans = player_transform.lock();
	const auto& param = parameter.lock();

	//親
	{
		const Vector2 left_axis = Input::Get_Pad_Axis_Left();

		const Vector3 now_pos = transform->Get_Position();
		Vector3 follow_pos = p_trans->Get_Position();
		follow_pos.y += 1;

		follow_pos.x = Mathf::Lerp(now_pos.x, follow_pos.x, follow_speed * (1 + abs(left_axis.y) * 0.75f) * Time::delta_time);
		follow_pos.y = Mathf::Lerp(now_pos.y, follow_pos.y, follow_speed * 0.75f * Time::delta_time);
		follow_pos.z = Mathf::Lerp(now_pos.z, follow_pos.z, follow_speed * (1 + abs(left_axis.y) * 0.75f) * Time::delta_time);

		transform->Set_Local_Position(follow_pos);

		const Vector2 axis = Input::Get_Pad_Axis_Right();
		Vector3 angle = { -axis.y * rotate_speed * Time::delta_time, -axis.x * rotate_speed * Time::delta_time, 0 };

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
		const auto& camera_trans = camera_transform.lock();
		camera_trans->Set_Local_Position(Vector3::Lerp(camera_trans->Get_Local_Position(), default_position, Time::delta_time));
		camera_trans->Set_Local_Euler_Angles(Vector3::Lerp(camera_trans->Get_Local_Euler_Angles(), default_rotation, Time::delta_time));
	}
}

void Player_Camera_Controller::Update_Battle()
{
	const auto& param = parameter.lock();
	const auto& e_manager = enemy_manager.lock();
	const auto& p_trans = player_transform.lock();
	const auto& camera_trans = camera_transform.lock();
	const auto& target = e_manager->last_attack_target.lock();

	param->target = target;
	const int enemy_count = e_manager->enemy_list.size();
	const Vector2 target_view_pos = camera.lock()->World_To_Viewport_Point(target->transform->Get_Position());

	//親
	{
		const Vector3 now_pos = transform->Get_Position();
		Vector3 follow_pos = p_trans->Get_Position();
		follow_pos.y += 1;

		follow_pos.x = Mathf::Lerp(now_pos.x, follow_pos.x, follow_speed * Time::delta_time);
		follow_pos.y = Mathf::Lerp(now_pos.y, follow_pos.y, follow_speed * 0.75f * Time::delta_time);
		follow_pos.z = Mathf::Lerp(now_pos.z, follow_pos.z, follow_speed * Time::delta_time);

		transform->Set_Local_Position(follow_pos);

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
			if (target_view_pos.x < 0.425f)
			{
				is_target_right = false;
			}
		}
		else
		{
			if (target_view_pos.x > 0.575f)
			{
				is_target_right = true;
			}
			pos.x *= -1;
			rot.y *= -1;
		}

		if (!param->is_ground)
		{
			pos.z -= 0.5f;
		}

		pos.z -= 0.5f * (Mathf::Clamp(static_cast<float>(enemy_count), 0, 4) - 1);

		camera_trans->Set_Local_Position(Vector3::Lerp(camera_trans->Get_Local_Position(), pos, Time::delta_time * 2.0f));

		camera_trans->Set_Local_Euler_Angles(Vector3::Lerp(camera_trans->Get_Local_Euler_Angles(), rot, Time::delta_time * 2.0f));
		const Vector3 r = camera_trans->Get_Euler_Angles();
		camera_trans->Set_Euler_Angles(r.x, r.y, 0);
	}
}

void Player_Camera_Controller::Update_Lock_On() const
{

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

		ImGui::LeftText_DragFloat3(u8"基本位置", "##Default_Position", default_position, window_center);
		ImGui::LeftText_DragFloat3(u8"基本向き", "##Default_Rotation", default_rotation, window_center);
		ImGui::LeftText_DragFloat3(u8"戦闘時位置", "##Battle_Position", battle_position, window_center);
		ImGui::LeftText_DragFloat3(u8"戦闘時向き", "##Battle_Rotation", battle_rotation, window_center);
		ImGui::LeftText_DragFloat3(u8"ロックオン時位置", "##Lock_Position", lock_position, window_center);
		ImGui::LeftText_DragFloat3(u8"ロックオン時向き", "##Lock_Rotation", lock_rotation, window_center);
	}
	return true;
}