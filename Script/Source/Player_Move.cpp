#include "Player_Move.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Player_Move::Awake()
{
	camera_transform = GameObject::Find_With_Tag("main_camera").lock()->transform;
	rigidbody = Get_Component<Capsule_Collider>()->rigidbody;
	animator = Get_Component<Animator>();
	parameter = Get_Component<Character_Parameter>();
}

void Player_Move::Move_Normal()
{
	Check_Move_Direction();

	const auto& rb = rigidbody.lock();
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
		speed = Vector3(0, rb->Get_Velocity().y, 0);
	}
	rb->Set_Velocity(speed);
}

void Player_Move::Move_Attack()
{
	Check_Move_Direction();

	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	speed.y = rb->Get_Velocity().y;
	rb->Set_Velocity(speed);
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

void Player_Move::Ground_Update()
{
	const auto& anim = animator.lock();
	if (anim->Get_Bool("Add_Jump_Force"))
	{
		Jump();
		anim->Set_Bool("Add_Jump_Force", false);
	}

	const auto& param = parameter.lock();
	if (!param->is_ground)
	{
		rigidbody.lock()->Add_Force(-transform->Get_Up() * down_power * Time::delta_time, Force_Mode::Force);
	}
}

void Player_Move::Jump() const
{
	rigidbody.lock()->Add_Force(transform->Get_Up() * jump_power, Force_Mode::Impulse);
}

void Player_Move::Check_Move_Direction()
{
	const auto& camera_trans = camera_transform.lock();

	// カメラの方向から、X-Z平面の単位ベクトルを取得
	camera_forward = camera_trans->Get_Forward();
	camera_forward.y = 0;
	camera_forward.Normalize();

	// 方向キーの入力値とカメラの向きから、移動方向を決定
	const Vector2 axis = Input::Get_Pad_Axis_Left();
	(camera_forward * axis.y - camera_trans->Get_Right() * axis.x).Normalize(move_forward);

	const auto& anim = animator.lock();
	if (move_forward == Vector3::Zero)
	{
		anim->Set_Bool("Move", false);
	}
	else
	{
		anim->Set_Bool("Move", true);
	}
	move_speed = anim->Get_Float("Move_Speed");
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