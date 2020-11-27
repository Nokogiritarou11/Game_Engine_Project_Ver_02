#include "Player.h"
using namespace std;
void Player::Awake()
{
}

void Player::Start()
{
	Cursor::lockState = CursorLockMode::Locked;
	jumppower = 5.0f;
	gravity = 0.01f;
	stage = GameObject::Find("Stage");
}

void Player::Update()
{	
	//入力
		static ImVec2 mouse_old_pos = { 0,0 };
		static ImVec2 mouse_pos = { 0,0 };
		Vector3 move = { 0,0,0 };
			mouse_pos = ImGui::GetMousePos();			
			if (mouse_old_pos.x == -1 && mouse_old_pos.y == -1)
			{
				mouse_old_pos = mouse_pos;
			}
			if (mouse_pos.x != mouse_old_pos.x || mouse_pos.y != mouse_old_pos.y)
			{
				const float dis_x = (mouse_pos.x - mouse_old_pos.x) * 0.1f;
				const float dis_y = (mouse_pos.y - mouse_old_pos.y) * 0.1f;

				Vector3 rot = transform->Get_eulerAngles();
				rot.y += dis_x;
				rot.x -= dis_y;
				rot.z = 0;
				transform->Set_eulerAngles(rot);

				mouse_old_pos = mouse_pos;
			}

		
		
			Vector3 front = transform->Get_forward();
		 Vector3 side=transform->Get_right();
		 side.Normalize();
		 front.Normalize();
		 
	if (GetAsyncKeyState('W')<0)
	{
		transform->Set_position(transform->Get_position()+Vector3(-front.x,0, -front.z));
	}
	if (GetAsyncKeyState('S') < 0)
	{
		transform->Set_position(transform->Get_position() + Vector3(front.x, 0, front.z));
	}
	if (GetAsyncKeyState('A') < 0)
	{
		transform->Set_position(transform->Get_position() + Vector3(side.x,0,side.z));
	}
	if (GetAsyncKeyState('D') < 0)
	{
		transform->Set_position(transform->Get_position() + Vector3(-side.x, 0, -side.z));
	}

	if (Input::GetKeyDown(KeyCode::Space))
	{
		if (!jumpflug)
		{
			jumpflug = true;
			jumpspeed = jumppower;
		}
	}
	if (jumpflug) { 
		jumpspeed -= gravity;
	}
	float feet = stage.lock()->transform->Get_localPosition().y;
	transform->Set_position(transform->Get_position() + Vector3(0,jumpspeed , 0));
	if (transform->Get_localPosition().y <= feet)
	{
		transform->Set_localPosition(Vector3(transform->Get_localPosition().x,feet,transform->Get_localPosition().z));
		jumpspeed = 0;
		jumpflug = false;
	}
	if (Input::GetMouseButtonDown(0))
	{
		//ショット
		Resources::Load_Prefab("Resouces/Prefab/bullet.prefab");
		// shared_ptr<GameObject> bullet = Instantiate("bullet.prefab", transform->Get_localPosition(), transform->Get_rotation());
	}
	if (Input::GetMouseButtonDown(1))

	{
		//ショット
		Resources::Load_Prefab("Resouces/Prefab/bomb.prefab");

		// shared_ptr<GameObject> bullet = Instantiate("bullet.prefab", transform->Get_localPosition(), transform->Get_rotation());
	}

	//if (Input_Manager::keyboard->Enter)
	//{
	//	GetComponent<AudioSource>()->PlayOneShot();
	//}
}

bool Player::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Player"))
	{
		if (ImGui::TreeNode("Jump"))
		{
			ImGui::SliderFloat("jumppower", &jumppower, 0.0f, 10.0f);
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