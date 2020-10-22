#include "Animator.h"
#include "Camera.h"
#include "Include_Mono.h"
#include "Include_ImGui.h"
#include "Player.h"
#include "Collider.h"
#include "Break_Block.h"
using namespace std;

void Player::Awake()
{
}

void Player::Start()
{
	Speed_Bonus_Magnification_Set = Speed_Bonus_Magnification;
	Speed_Bonus_Magnification = 1;
	Gas = Gas_Max;
	shared_ptr<Animator> anim = GetComponent<Animator>();
	anim->loopAnimation = true;
	anim->Play(0);
	/*
	blocks_count = 8;
	for (int i = 0; i < blocks_count; ++i)
	{
		shared_ptr<GameObject> breaks = GameObject::Instantiate(u8"breaks_" + to_string(i));
		breaks->AddComponent<Break_Block>();
		breaks->SetActive(true);
	}*/
}

void Player::Update()
{
	if (Stage_State == 4)
	{
		Check_Player_Move();
		Check_Parameter();
		Move();
	}
}

/*
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
*/

void Player::Check_Player_Move()
{
	/*
	if (Input_Manager::pad.connected)
	{
		Horizontal = Input_Manager::pad.thumbSticks.leftX;
		Vertical = Input_Manager::pad.thumbSticks.leftY;
	}
	else
	{
		if (Input_Manager::kb.Right) Horizontal = 1;
		if (Input_Manager::kb.Left)   Horizontal = -1;
		if (!Input_Manager::kb.Right && !Input_Manager::kb.Left) Horizontal = 0;

		if (Input_Manager::kb.Up) Vertical = 1;
		if (Input_Manager::kb.Down) Vertical = -1;
		if (!Input_Manager::kb.Up && !Input_Manager::kb.Down) Vertical = 0;
	}

	// 方向キーの入力値から、移動方向を決定
	moveForward = Vector3::Forward * Vertical + Vector3::Right * -Horizontal;

	if (moveForward == Vector3::Zero)
	{
		moveForward = transform->Get_forward();
	}

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
	*/
	if (Input_Manager::pad.connected)
	{
		Horizontal = Input_Manager::pad.thumbSticks.leftX;
	}
	else
	{
		float Horizontal_Power = 7.5f * Time::deltaTime;
		if (Input_Manager::kb.Right)  Horizontal += Horizontal_Power;
		if (Input_Manager::kb.Left)   Horizontal -= Horizontal_Power;
		Horizontal = Mathf::Clamp(Horizontal, -1, 1);

		if (!Input_Manager::kb.Right && !Input_Manager::kb.Left)
		{
			if (Horizontal > 0.1f)
			{
				Horizontal -= Horizontal_Power;
			}
			else if (Horizontal < -0.1f)
			{
				Horizontal += Horizontal_Power;
			}
			else
			{
				Horizontal = 0;
			}
		}
	}
	transform->Set_eulerAngles(0, 0, -30 * Horizontal);

	if (!Boosting && Can_Boost && (Input_Manager::key_tracker.IsKeyPressed(Keyboard::Space) || Input_Manager::pad_tracker.a == GamePad::ButtonStateTracker::PRESSED))
	{
		Boosting = true;
		Can_Boost = false;
		Boost_Timer = Boost_Time_Max;
		Audio_Manager::Boost->Play();
	}

	moveForward = Vector3::Right * Horizontal;
}

void Player::Check_Parameter()
{
	if (Gas > 100)
	{
		Gas = 100;
	}
	else if (Gas > 0)
	{
		Gas -= Gas_Decrease * Time::deltaTime;
	}
	else
	{
		Gas = 0;
	}

	if (!Can_Boost)
	{
		if (Boost_Charge_Timer < Boost_Charge_Time_Max)
		{
			Boost_Charge_Timer += Time::deltaTime;
		}
		else
		{
			Boost_Charge_Timer = 0;
			Can_Boost = true;
		}
	}

	if (Invincible)
	{
		Invincible_Timer += Time::deltaTime * 2;
		if ((int)(Invincible_Timer * 2.0f) % 2 == 1)
		{
			if (!skin_enabled)
			{
				GetComponent<SkinMesh_Renderer>()->enabled = true;
				skin_enabled = true;
			}
		}
		else
		{
			if (skin_enabled)
			{
				GetComponent<SkinMesh_Renderer>()->enabled = false;
				skin_enabled = false;
			}
		}
		if (Invincible_Timer > 6.0f)
		{
			Invincible = false;
			Invincible_Timer = 0;
			if (!skin_enabled)
			{
				GetComponent<SkinMesh_Renderer>()->enabled = true;
				skin_enabled = true;
			}
		}
	}

	if (Boosting)
	{
		Boost_Timer -= Time::deltaTime;
		if (Boost_Timer <= 0)
		{
			Boosting = false;
		}
	}

	if (!Damage)
	{
		Speed_Bonus_Timer += Time::deltaTime;
		if (Speed_Bonus_Timer > Speed_Bonus_Time_Max)
		{
			Speed_Bonus_Count++;
			Speed_Bonus_Timer = 0;
		}
	}
	else
	{
		Speed_Bonus_Timer = 0;
		Speed_Bonus_Count = 0;

		Damage = false;
	}
	Speed_Bonus_Magnification = 1 + (Speed_Bonus_Magnification_Set * Speed_Bonus_Count);
}

void Player::Move()
{
	/*
	if (Move_Speed > 0)
	{
		Vector3 to_pos = transform->Get_position() + transform->Get_forward() * Move_Speed * Time::deltaTime;
		bool cant_move = false;
		for (size_t i = 0; i < colliders.size(); ++i)
		{
			shared_ptr<Collider> col = colliders[i].lock();
			if (col->gameObject->activeSelf())
			{
				float x_min = col->transform->Get_position().x - col->Size_X;
				float x_max = col->transform->Get_position().x + col->Size_X;
				float z_min = col->transform->Get_position().z - col->Size_Z;
				float z_max = col->transform->Get_position().z + col->Size_Z;

				float pos_x = to_pos.x;
				float pos_z = to_pos.z;
				if (pos_x > x_min && pos_x < x_max && pos_z < z_max && pos_z > z_min)
				{
					cant_move = true;
				}
			}
		}
		if (!cant_move && to_pos.x > -3.0f && to_pos.x < 3.0f)
		{
			transform->Set_position(to_pos);
		}
		Move_Speed -= Down_Power * Time::deltaTime;
	}
	else
	{
		Move_Speed = 0;
	}
	*/

	Vector3 to_pos = transform->Get_position();
	to_pos += moveForward * Move_Speed * Time::deltaTime;

	for (size_t i = 0; i < colliders.size(); ++i)
	{
		shared_ptr<Collider> col = colliders[i].lock();
		if (col->gameObject->activeSelf())
		{
			/*
			float x_min = col->transform->Get_position().x - col->Size_X;
			float x_max = col->transform->Get_position().x + col->Size_X;
			float z_min = col->transform->Get_position().z - col->Size_Z;
			float z_max = col->transform->Get_position().z + col->Size_Z;

			float pos_x = to_pos.x;
			float pos_z = to_pos.z;
			if (pos_x > x_min && pos_x < x_max && pos_z < z_max && pos_z > z_min)
			*/
			Vector2 t = { col->transform->Get_position().x,col->transform->Get_position().z };
			Vector2 p = { transform->Get_position().x,transform->Get_position().z };
			if (Vector2::Distance(p, t) < col->Size_X)
			{
				if (col->obj_type == Collider::Block)
				{
					if (Boosting)
					{
						col->gameObject->SetActive(false);
						Audio_Manager::Clash->Play();
					}
					else if (!Invincible)
					{
						Damage = true;
						Gas -= Gas_Damage;
						Invincible = true;
						Audio_Manager::Damage->Play();
						/*
						for (int i = 0; i < blocks_count; ++i)
						{
							shared_ptr<GameObject> obj = blocks[i].lock();
							if (!obj->activeSelf())
							{
								obj->SetActive(true);
								obj->transform->Set_position(transform->Get_position());
								break;
							}
						}*/
					}
				}
				else if (col->obj_type == Collider::Gas)
				{
					Audio_Manager::Get->Play();
					col->gameObject->SetActive(false);
					Gas += Gas_Increase;
				}
				else if (col->obj_type == Collider::Bonus)
				{
					Audio_Manager::Get->Play();
					col->gameObject->SetActive(false);
					Get_Bonus = true;
				}
			}
		}
	}

	if (to_pos.x > -2.8f && to_pos.x < 2.8f)
	{
		transform->Set_position(to_pos);
	}
}

bool Player::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Player"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Player_sub"))
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
		ImGui::Text(u8"パラメータ変数");
		//ImGui::DragFloat(u8"applySpeed", &applySpeed, 0.1f, 0.01f, FLT_MAX);
		ImGui::DragFloat(u8"Move_Speed", &Move_Speed, 0.1f, 0, FLT_MAX);
		ImGui::DragFloat(u8"Boost_Magnification", &Boost_Magnification, 0.1f, 1.0f, FLT_MAX);
		ImGui::DragFloat(u8"Boost_Time_Max", &Boost_Time_Max, 0.1f, 0, FLT_MAX);
		ImGui::DragFloat(u8"Boost_Charge_Time_Max", &Boost_Charge_Time_Max, 0.1f, 0, FLT_MAX);
		ImGui::DragFloat(u8"Gas_Max", &Gas_Max, 0.1f, 0, FLT_MAX);
		ImGui::DragFloat(u8"Gas_Decrease", &Gas_Decrease, 0.1f, 0, FLT_MAX);
		ImGui::DragFloat(u8"Gas_Increase", &Gas_Increase, 0.1f, 0, FLT_MAX);
		ImGui::DragFloat(u8"Gas_Damage", &Gas_Damage, 0.1f, 0, FLT_MAX);
		ImGui::DragFloat(u8"Speed_Bonus_Magnification", &Speed_Bonus_Magnification, 0.1f, 1.0f, FLT_MAX);
		ImGui::DragFloat(u8"Speed_Bonus_Time_Max", &Speed_Bonus_Time_Max, 0.1f, 0, FLT_MAX);
		//ImGui::DragFloat(u8"Max_Speed", &Max_Speed, 0.1f, -FLT_MAX, FLT_MAX);
		//ImGui::DragFloat(u8"Add_Power", &Add_Power, 0.1f, -FLT_MAX, FLT_MAX);
		//ImGui::DragFloat(u8"Down_Power", &Down_Power, 0.1f, -FLT_MAX, FLT_MAX);

		ImGui::Text(u8"デバッグ用");
		ImGui::Text(u8"Gas : %.1f", Gas);
		//ImGui::Text(u8"Move_Speed : %.2f", Move_Speed);
		//ImGui::Text(u8"Dash_Power : %.2f", Dash_Power);
	}
	return true;
}