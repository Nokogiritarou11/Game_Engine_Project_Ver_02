#include "Player.h"
using namespace std;
void Player::Awake()
{
}

void Player::Start()
{
	Cursor::lockState = CursorLockMode::Locked;
}

void Player::Update()
{
	Move();

	if (Input::GetKeyDown(KeyCode::Space))
	{
		if (!jump_flg)
		{
			jump_flg = true;
			jump_speed = jump_power;
		}
	}
	if (jump_flg)
	{
		jump_speed -= gravity;
		transform->Set_position(transform->Get_position() + Vector3(0, jump_speed, 0));
	}
	if (transform->Get_localPosition().y <= 0)
	{
		transform->Set_localPosition(Vector3(transform->Get_localPosition().x, 0, transform->Get_localPosition().z));
		jump_speed = 0;
		jump_flg = false;
	}

	if (Input::GetMouseButtonDown(0))
	{
		//ショット
		//shared_ptr<GameObject> bullet = Resources::Load_Prefab("Resouces/Prefab/bullet.prefab");;
	}
	if (Input::GetMouseButtonDown(1))
	{
		//ショット
		//shared_ptr<GameObject> bullet = Resources::Load_Prefab("Resouces/Prefab/bomb.prefab");;
	}
}

void Player::Move()
{
	//入力
	static Vector2 mouse_pos = { 0,0 };
	mouse_pos = Input::GetMouseRelativePosition();
	Debug::Log(mouse_pos);
	Vector3 rot = transform->Get_eulerAngles();
	rot.y += mouse_pos.x;
	rot.x += mouse_pos.y;
	rot.z = 0;
	transform->Set_eulerAngles(rot);

	static Vector3 move = { 0,0,0 };
	static Vector2 move_input = { 0,0 };
	move = { 0,0,0 };
	move_input = { 0,0 };
	Vector3 front = transform->Get_forward();
	Vector3 side = transform->Get_right();

	front.y = 0;
	front.Normalize();

	if (Input::GetKey(KeyCode::W))
	{
		move_input.y += 1;
	}
	if (Input::GetKey(KeyCode::S))
	{
		move_input.y -= 1;
	}
	if (Input::GetKey(KeyCode::A))
	{
		move_input.x -= 1;
	}
	if (Input::GetKey(KeyCode::D))
	{
		move_input.x += 1;
	}

	transform->Set_localPosition(transform->Get_localPosition() + front * move_input.y + side * move_input.x);

}

bool Player::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Player"))
	{
		if (ImGui::TreeNode("Jump"))
		{
			ImGui::SliderFloat("jump_power", &jump_power, 0.0f, 10.0f);
			ImGui::SliderFloat("gravity", &gravity, 0.0f, 10.0f);

			ImGui::TreePop();
		}
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
	}
	return true;
}