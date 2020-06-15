#include "Player.h"
#include "Animator.h"
#include "Camera.h"
#include "Scene_Manager.h"

void Player::Awake()
{
	transform->Set_position(0, 75, 0.0f);
	transform->Set_eulerAngles(180, 0, 0);
	transform->Set_scale(0.1f, 0.1f, 0.1f);
}

void Player::Start()
{
	move_speed = 30;
	rotate_speed = 70.0f;
	jump_power = 30.0f;
	down_speed = 25.0;
}

void Player::Update()
{
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

	/////////////////////////////////////////////////////////@@’Ç‰Á•”•ª
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
	transform->Set_position(set_pos);

}