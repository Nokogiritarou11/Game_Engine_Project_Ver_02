#include "Player.h"
#include "Object_Pool.h"
using namespace std;

void Player::Awake()
{
	Cursor::lockState = CursorLockMode::Locked;
	HP = 100;
}

void Player::Start()
{
	obj_pool = GameObject::Find("Object_Pool").lock()->GetComponent<Object_Pool>();
	muzzle_trans = GameObject::Find("Muzzle").lock()->transform;
	muzzle_flash = GameObject::Find("Muzzle_Flash").lock()->GetComponent<Particle>();
	se_shot = GameObject::Find("Muzzle_Flash").lock()->GetComponent<AudioSource>();
}

void Player::Update()
{
	if (HP > 0)
	{
		Move();
		Jump();
		Shot();
	}
	else
	{

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
	set += transform->Get_localPosition();
	transform->Set_localPosition(set);

	if (Vector2::Distance(Vector2(set.x, set.z), Vector2::Zero) >= 320.0f)
	{
		Vector3 to = Vector3::Zero - set;
		to.Normalize();
		transform->Set_position(set + to);
	}
}

void Player::Jump()
{
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
}

void Player::Shot()
{
	static float shot_timer = 0;
	shot_timer += Time::deltaTime;
	if (shot_timer >= 0.15f && Input::GetMouseButton(0))
	{
		//ショット
		shared_ptr<GameObject> bullet = obj_pool.lock()->Instance_Bullet();
		if (bullet)
		{
			muzzle_flash.lock()->Play();
			se_shot.lock()->PlayOneShot();
			bullet->transform->Set_position(muzzle_trans.lock()->Get_position());
			bullet->transform->Set_rotation(muzzle_trans.lock()->Get_rotation());
			shot_timer = 0;
		}
	}

	static float bomb_timer = 0;
	bomb_timer += Time::deltaTime;
	if (bomb_timer >= 2.0f && Input::GetMouseButtonDown(1))
	{
		//ボム
		shared_ptr<GameObject> bullet = obj_pool.lock()->Instance_Bomb();
		if (bullet)
		{
			muzzle_flash.lock()->Play();
			se_shot.lock()->PlayOneShot();
			bullet->transform->Set_position(muzzle_trans.lock()->Get_position());
			Vector3 e = muzzle_trans.lock()->Get_eulerAngles();
			bullet->transform->Set_eulerAngles(e.x - 5.0f, e.y, e.z);
			bomb_timer = 0;
		}
	}
}

void Player::Damage(int damage)
{
	HP -= damage;
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
		ImGui::DragFloat("move_speed", &move_speed, 0.1f, 0.0f, FLT_MAX);
		ImGui::DragFloat("aim_speed", &aim_speed, 0.1f, 0.0f, FLT_MAX);
		ImGui::DragFloat("jump_power", &jump_power, 0.1f, 0.0f, FLT_MAX);
		ImGui::DragFloat("gravity", &gravity, 0.1f, 0.0f, FLT_MAX);
	}
	return true;
}