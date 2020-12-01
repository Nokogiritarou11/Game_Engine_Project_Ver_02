#include "Player.h"
#include "Object_Pool.h"
using namespace std;

void Player::Awake()
{
	Cursor::lockState = CursorLockMode::Locked;
}

void Player::Start()
{
	obj_pool = GameObject::Find("Object_Pool").lock()->GetComponent<Object_Pool>();
	muzzle_trans = GameObject::Find("Muzzle").lock()->transform;
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
		jump_speed -= gravity * Time::deltaTime;
		transform->Set_position(transform->Get_position() + Vector3(0, jump_speed * Time::deltaTime, 0));
		if (transform->Get_localPosition().y < 0)
		{
			transform->Set_localPosition(Vector3(transform->Get_localPosition().x, 0, transform->Get_localPosition().z));
			jump_speed = 0;
			jump_flg = false;
		}
	}

	if (Input::GetMouseButtonDown(0))
	{
		//ショット
		shared_ptr<GameObject> bullet = obj_pool.lock()->Instance_Bullet().lock();
		bullet->transform->Set_position(muzzle_trans.lock()->Get_position());
		bullet->transform->Set_rotation(muzzle_trans.lock()->Get_rotation());
	}
	if (Input::GetMouseButtonDown(1))
	{
		//ショット
		shared_ptr<GameObject> bullet = obj_pool.lock()->Instance_Bomb().lock();
		bullet->transform->Set_position(muzzle_trans.lock()->Get_position());
		bullet->transform->Set_rotation(muzzle_trans.lock()->Get_rotation());
	}
}

void Player::Move()
{
	//入力
	static Vector2 mouse_pos = { 0,0 };
	mouse_pos = Input::GetMouseRelativePosition();
	Vector3 rot = transform->Get_eulerAngles();
	rot.y += mouse_pos.x * aim_speed * Time::deltaTime;
	rot.x += mouse_pos.y * aim_speed * Time::deltaTime;
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
		move_input.y -= 1;
	}
	if (Input::GetKey(KeyCode::S))
	{
		move_input.y += 1;
	}
	if (Input::GetKey(KeyCode::A))
	{
		move_input.x += 1;
	}
	if (Input::GetKey(KeyCode::D))
	{
		move_input.x -= 1;
	}

	Vector3 set = front * move_input.y * move_speed * Time::deltaTime + side * move_input.x * move_speed * Time::deltaTime;
	set.y = 0;
	transform->Set_localPosition(transform->Get_localPosition() + set);

}

bool Player::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Player");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Player_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Player>(shared_from_this()));
			removed = false;
		}
		ImGui::EndPopup();
	}
	if (!removed)
	{
		return false;
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
	static bool enable;
	enable = enableSelf();
	if (ImGui::Checkbox("##enable", &enable))
	{
		SetEnabled(enable);
	}

	if (open)
	{
		ImGui::Checkbox("jump", &jump_flg);
		ImGui::DragFloat("move_speed", &move_speed, 0.1f, 0.0f, FLT_MAX);
		ImGui::DragFloat("aim_speed", &aim_speed, 0.1f, 0.0f, FLT_MAX);
		ImGui::DragFloat("jump_power", &jump_power, 0.1f, 0.0f, FLT_MAX);
		ImGui::DragFloat("gravity", &gravity, 0.1f, 0.0f, FLT_MAX);
	}
	return true;
}