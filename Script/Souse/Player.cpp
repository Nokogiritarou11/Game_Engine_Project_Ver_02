#include "Animator.h"
#include "Camera.h"
#include "Include_Mono.h"
#include "Player.h"
using namespace std;

void Player::Awake()
{
	transform->Set_position(0, 0, 0.0f);
	//transform->Set_eulerAngles(-90, 0, 0);
	transform->Set_eulerAngles(180, 0, 0);
	transform->Set_scale(0.1f, 0.1f, 0.1f);
}

void Player::Start()
{
	move_speed = 30;
	rotate_speed = 70.0f;
	//jump_power = 30.0f;
	down_speed = 25.0;

	//shared_ptr<GameObject> g = GameObject::FindWithTag("ObjectPool").lock();
	//pool = g->GetComponent<ObjectPool>();
}

void Player::Update()
{

	transform->Set_position(transform->Get_position()+transform->Get_right() * Time::deltaTime);
	/*
	float L = Input_Manager::kb.Left;
	float R = Input_Manager::kb.Right;
	float U = Input_Manager::kb.Up;
	float D = Input_Manager::kb.Down;

	Vector3 rot = transform->Get_eulerAngles();
	if (L)
	{
		rot.y -= rotate_speed * Time::deltaTime;
	}
	if (R)
	{
		rot.y += rotate_speed * Time::deltaTime;
	}
	transform->Set_eulerAngles(rot);

	set_pos = transform->Get_position();
	if (U)
	{
		set_pos += transform->Get_forward() * move_speed * Time::deltaTime;
	}
	if (D)
	{
		set_pos -= transform->Get_forward() * move_speed * Time::deltaTime;
	}

	transform->Set_position(set_pos);

	if (Input_Manager::key_tracker.pressed.Space)
	{
		shared_ptr<ObjectPool> p = pool.lock();
		Vector3 pos = transform->Get_position() + transform->Get_forward() * 7.5f;
		pos.y = 5.5f;
		p->Instance_inPool(0, pos, transform->Get_rotation());
	}
	*/
	/////////////////////////////////////////////////////////@@’Ç‰Á•”•ª
	/*
	if (Input_Manager::key_tracker.pressed.Space && !Jumping)
	{
		jump_speed = jump_power;
		Jumping = true;
	}
	if (Jumping)
	{
		jump_speed -= down_speed * Time::deltaTime;
	}


	if (set_pos.y >= 0 && Jumping)
	{
		set_pos.y += jump_speed * Time::deltaTime;
	}
	else
	{
		set_pos.y = 0;
		Jumping = false;
	}
	*/

	/*
	//Input_Manager::mouse->SetMode(Mouse::MODE_RELATIVE);
	Vector3 move_pos = { 0,0,0 };
	if (Input_Manager::kb.W)
	{
		move_pos = transform->Get_position() + transform->Get_up() * 300 * Time::deltaTime;
		transform->Set_position(Vector3::Lerp(transform->Get_position(), move_pos, Time::deltaTime * 10));
	}
	if (Input_Manager::kb.S)
	{
		move_pos = transform->Get_position() - transform->Get_up() * 300 * Time::deltaTime;
		transform->Set_position(Vector3::Lerp(transform->Get_position(), move_pos, Time::deltaTime * 10));
	}
	if (Input_Manager::kb.D)
	{
		move_pos = transform->Get_position() - transform->Get_right() * 300 * Time::deltaTime;
		transform->Set_position(Vector3::Lerp(transform->Get_position(), move_pos, Time::deltaTime * 10));
	}
	if (Input_Manager::kb.A)
	{
		move_pos = transform->Get_position() + transform->Get_right() * 300 * Time::deltaTime;
		transform->Set_position(Vector3::Lerp(transform->Get_position(), move_pos, Time::deltaTime * 10));
	}
	float L = Input_Manager::kb.Left;
	float R = Input_Manager::kb.Right;
	float U = Input_Manager::kb.Up;
	float D = Input_Manager::kb.Down;

	Vector3 rot = { -90,transform->Get_eulerAngles().y ,0 };
	if (L)
	{
		rot.y -= rotate_speed * Time::deltaTime;
	}
	if (R)
	{
		rot.y += rotate_speed * Time::deltaTime;
	}
	transform->Set_eulerAngles(rot);

	*/
	/*
	static Vector3 rot = { 0,0,0 };
	//rot.x = Input_Manager::ms.y;
	rot.y = Input_Manager::ms.x;
	transform->Set_eulerAngles(transform->Get_eulerAngles() + rot);
	*/
}