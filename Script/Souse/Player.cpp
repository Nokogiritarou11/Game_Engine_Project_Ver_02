#include "Animator.h"
#include "Camera.h"
#include "Include_Mono.h"
#include "Include_ImGui.h"
#include "Player.h"
using namespace std;

void Player::Awake()
{
}

void Player::Start()
{
}

void Player::Update()
{
	Check_Player_Move();
	Move();
}

Quaternion LookRotation(const Vector3& direction, const Vector3& up = { 0, 1, 0 })
{
	using namespace DirectX;
	Vector3 xaxis, yaxis;
	up.Cross(direction, xaxis);
	xaxis.Normalize();

	direction.Cross(xaxis, yaxis);
	yaxis.Normalize();

	Matrix res = { xaxis.x ,yaxis.x ,direction.x ,0 ,
				   xaxis.y ,yaxis.y ,direction.y ,0 ,
				   xaxis.z ,yaxis.z ,direction.z ,0 ,
						 0 ,	  0 ,		   0 ,1 };
	Quaternion out = Quaternion::CreateFromRotationMatrix(res);
	return out;
}

void Player::Check_Player_Move()
{

	if (Input_Manager::pad.connected)
	{
		Horizontal = Input_Manager::pad.thumbSticks.leftX;
		Vertical = Input_Manager::pad.thumbSticks.leftY;
	}
	else
	{
		if (Input_Manager::kb.Right) Horizontal = 1;
		if (Input_Manager::kb.Left)   Horizontal = -1;

		if (Input_Manager::kb.Up) Vertical = 1;
		if (Input_Manager::kb.Down) Vertical = -1;
	}

	// 方向キーの入力値とカメラの向きから、移動方向を決定
	moveForward = Vector3::Forward * Vertical + Vector3::Right * -Horizontal;

	if (Input_Manager::kb.Space || Input_Manager::pad.buttons.a)
	{
		transform->Set_rotation(Quaternion::Slerp(transform->Get_rotation(), LookRotation(moveForward), applySpeed * Time::deltaTime));//滑らかに方向転換
		if (Dash_Power < Max_Speed)
		{
			Dash_Power += Add_Power * Time::deltaTime;
		}
		else
		{
			Dash_Power = Max_Speed;
		}
	}
	if (Input_Manager::key_tracker.IsKeyReleased(Keyboard::Space) || Input_Manager::pad_tracker.a == GamePad::ButtonStateTracker::RELEASED)
	{
		Move_Speed = Dash_Power;
		Dash_Power = 0;
	}
}

void Player::Move()
{
	transform->Set_position(transform->Get_position() + Vector3(0, 0, -Scroll_Speed * Time::deltaTime));
	if (Move_Speed > 0)
	{
		transform->Set_position(transform->Get_position() + transform->Get_forward() * Move_Speed * Time::deltaTime);
		Move_Speed -= Down_Power * Time::deltaTime;
	}
	else
	{
		Move_Speed = 0;
	}
}

bool Player::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Camera"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Camera_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<Player>(shared_from_this()));
				removed = false;
			}
			ImGui::EndPopup();
			if (!removed)
			{
				return false;
			}
		}

		ImGui::DragFloat(u8"applySpeed", &applySpeed, 0.1f, 0.01f, FLT_MAX);
		ImGui::DragFloat(u8"Move_Speed", &Move_Speed, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat(u8"Max_Speed", &Max_Speed, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat(u8"Scroll_Speed", &Scroll_Speed, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat(u8"Add_Power", &Add_Power, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat(u8"Down_Power", &Down_Power, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat(u8"Dash_Power", &Dash_Power, 0.1f, -FLT_MAX, FLT_MAX);
	}
	return true;
}