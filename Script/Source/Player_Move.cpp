#include "Player_Move.h"

using namespace std;
using namespace BeastEngine;

void Player_Move::Move_Normal()
{
	Check_Move_Direction();

	auto& rb = rigidbody.lock();
	Vector3 speed;

	if (move_forward != Vector3::Zero)
	{
		//滑らかに方向転換
		transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Local_Rotation(), transform->Look_At(transform->Get_Position() + move_forward), turn_speed * Time::delta_time));

		speed = transform->Get_Forward() * run_speed * Time::delta_time;
		speed.y = rb->Get_Velocity().y;
	}
	else
	{
		speed = { 0, rb->Get_Velocity().y , 0 };
	}
	rb->Set_Velocity(speed);
}

void Player_Move::Move_Attack()
{

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

void Player_Move::Awake()
{
	camera_transform = GameObject::Find_With_Tag("main_camera").lock()->transform;
	rigidbody = Get_Component<Capsule_Collider>()->rigidbody;
	animator = Get_Component<Animator>();
}

void Player_Move::Check_Move_Direction()
{
	auto& camera_trans = camera_transform.lock();

	// カメラの方向から、X-Z平面の単位ベクトルを取得
	camera_forward = camera_trans->Get_Forward();
	camera_forward.y = 0;
	camera_forward.Normalize();

	// 方向キーの入力値とカメラの向きから、移動方向を決定
	const Vector3 axis = Input::Get_Pad_Axis_Left();
	(camera_forward * axis.y - camera_trans->Get_Right() * axis.x).Normalize(move_forward);

	auto& anim = animator.lock();
	if (move_forward == Vector3::Zero)
	{
		anim->Set_Bool("Move", false);
	}
	else
	{
		anim->Set_Bool("Move", true);
	}
}

bool Player_Move::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Player_Move", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	if (ImGui::BeginPopupContextItem("Player_Move_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Player_Move>(shared_from_this()));
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

		ImGui::Text("Run_Speed");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat("##Run_Speed", &run_speed, 0.01f);

		ImGui::Text("Turn_Speed");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat("##Turn_Speed", &turn_speed, 0.01f);
	}
	return true;
}