#include "Player_Camera_Controller.h"

using namespace std;
using namespace BeastEngine;

void Player_Camera_Controller::Awake()
{
	player_transform = GameObject::Find_With_Tag("player").lock()->transform;
}

void Player_Camera_Controller::Update()
{
	auto& p_trans = player_transform.lock();

	Vector3 follow_pos = p_trans->Get_Position();
	follow_pos.y += 1;
	transform->Set_Local_Position(Vector3::Lerp(transform->Get_Position(), follow_pos, follow_speed * Time::delta_time));

	Vector2 axis = Input::Get_Pad_Axis_Right();
	Vector3 angle = { -axis.y * rotate_speed * Time::delta_time, -axis.x * rotate_speed * Time::delta_time, 0 };
	Vector3 rot = transform->Get_Euler_Angles() + angle;

	//最大角度にクランプ
	float angle_x = 180.0f <= rot.x ? rot.x - 360 : rot.x;
	transform->Set_Local_Euler_Angles(Mathf::Clamp(angle_x, angle_limit_down, angle_limit_up), rot.y, rot.z);
}

bool Player_Camera_Controller::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Player_Camera_Controller", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	if (ImGui::BeginPopupContextItem("Player_Camera_Controller_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Player_Camera_Controller>(shared_from_this()));
			removed = true;
		}
		ImGui::EndPopup();
	}
	if (removed)
	{
		return false;
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 20.0f);
	bool enable = Get_Enabled();
	if (ImGui::Checkbox("##enable", &enable))
	{
		Set_Enabled(enable);
	}

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.6f;

		ImGui::Text("Angle_Limit_Up");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat("##Angle_Limit_Up", &angle_limit_up, 0.01f);

		ImGui::Text("Angle_Limit_Down");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat("##Angle_Limit_Down", &angle_limit_down, 0.01f);

		ImGui::Text("Rotate_Speed");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat("##Rotate_Speed", &rotate_speed, 0.01f);

		ImGui::Text("Follow_Speed");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat("##Follow_Speed", &follow_speed, 0.01f);
	}
	return true;
}