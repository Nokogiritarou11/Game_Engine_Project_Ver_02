#include "Player_Camera_Controller.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Player_Camera_Controller::Awake()
{
	shared_ptr<GameObject>& obj = GameObject::Find_With_Tag("player").lock();
	player_transform = obj->transform;
	parameter = obj->Get_Component<Character_Parameter>();
}

void Player_Camera_Controller::LateUpdate()
{
	auto& p_trans = player_transform.lock();
	auto& param = parameter.lock();

	Vector2 left_axis = Input::Get_Pad_Axis_Left();

	Vector3 now_pos = transform->Get_Position();
	Vector3 follow_pos = p_trans->Get_Position();
	follow_pos.y += 1;

	follow_pos.x = Mathf::Lerp(now_pos.x, follow_pos.x, follow_speed * (1 + abs(left_axis.y) * 0.75f) * Time::delta_time);
	follow_pos.y = Mathf::Lerp(now_pos.y, follow_pos.y, follow_speed * 0.75f * Time::delta_time);
	follow_pos.z = Mathf::Lerp(now_pos.z, follow_pos.z, follow_speed * (1 + abs(left_axis.y) * 0.75f) * Time::delta_time);

	transform->Set_Local_Position(follow_pos);

	Vector2 axis = Input::Get_Pad_Axis_Right();
	Vector3 angle = { -axis.y * rotate_speed * Time::delta_time, -axis.x * rotate_speed * Time::delta_time, 0 };

	if (param->moveing)
	{
		angle.y -= left_axis.x * rotate_speed * 0.25f * Time::delta_time;
	}

	Vector3 rot = transform->Get_Euler_Angles() + angle;

	//Å‘åŠp“x‚ÉƒNƒ‰ƒ“ƒv
	float angle_x = 180.0f <= rot.x ? rot.x - 360 : rot.x;
	transform->Set_Local_Euler_Angles(Mathf::Clamp(angle_x, angle_limit_down, angle_limit_up), rot.y, rot.z);
}

bool Player_Camera_Controller::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Player_Camera_Controller", open)) return false;

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"ã•ûŒüŠp“xÅ‘å", "##Angle_Limit_Up", &angle_limit_up, window_center);
		ImGui::LeftText_DragFloat(u8"‰º•ûŒüŠp“xÅ‘å", "##Angle_Limit_Down", &angle_limit_down, window_center);
		ImGui::LeftText_DragFloat(u8"‰ñ“]‘¬“x", "##Rotate_Speed", &rotate_speed, window_center);
		ImGui::LeftText_DragFloat(u8"’Ç]‘¬“x", "##Follow_Speed", &follow_speed, window_center);
	}
	return true;
}