#include "Camera_Set.h"
#include "Debug.h"

void Camera_Set::Awake()
{
	Camera_State = 0;
	transform->Set_eulerAngles(0, 90, 0);
}
void Camera_Set::Start()
{
	weak_ptr<GameObject> g = GameObject::FindWithTag("Player");
	shared_ptr<GameObject> _g = g.lock();
	player_transform = _g->GetComponent<Transform>();
}
void Camera_Set::LateUpdate()
{
	if (Input_Manager::key_tracker.pressed.Enter)
	{
		if (Camera_State < 5)
		{
			Camera_State++;
		}
		else
		{
			Camera_State = 0;
			Input_Manager::mouse->SetMode(Mouse::MODE_ABSOLUTE);
		}
		Debug::Log(Camera_State);
	}
	///*
	shared_ptr<Transform> p = player_transform.lock();
	switch (Camera_State)
	{
		case 0:
			Surveillance(p->Get_position());
			break;
		case 1:
			Fixed_relative_position(p->Get_position());
			break;
		case 2:
			TPS(p);
			break;
		case 3:
			FPS(p);
			break;
		case 4:
			Chase(p->Get_position());
			break;
		case 5:
			Free();
			break;
		default:
			break;
	}
	//*/
}

void Camera_Set::Surveillance(Vector3 target)
{
	move_pos = { 0, 75, -125.0f };
	transform->Set_position(Vector3::Lerp(transform->Get_position(), move_pos, Time::deltaTime * 10));
	transform->Set_rotation(transform->LookAt(target));
}
void Camera_Set::Fixed_relative_position(Vector3 target)
{
	Vector3 offset = { 100,20,-50 };
	move_pos = target + offset;
	transform->Set_position(Vector3::Lerp(transform->Get_position(), move_pos, Time::deltaTime * 10));
	transform->Set_rotation(transform->LookAt(target));
}
void Camera_Set::TPS(shared_ptr<Transform> parent)
{
	Vector3 offset = { 0,50,0 };
	offset += parent->Get_forward() * -100;
	move_pos = parent->Get_position() + offset;
	transform->Set_position(Vector3::Lerp(transform->Get_position(), move_pos, Time::deltaTime * 10));
	transform->Set_rotation(transform->LookAt(parent->Get_position()));
}
void Camera_Set::FPS(shared_ptr<Transform> parent)
{
	Vector3 offset = { 0,7.5f,0 };
	move_pos = parent->Get_position() + offset;
	transform->Set_position(move_pos);
	transform->Set_rotation(parent->Get_rotation());
}
void Camera_Set::Chase(Vector3 target)
{
	Vector3 v = transform->Get_position();
	v.y = target.y;
	float dis = Vector3::Distance(v, target);
	if (dis > 200.0f || dis < 50.0f)
	{
		Vector3 offset = { 100,50,50 };
		move_pos = offset + target;
	}
	transform->Set_position(Vector3::Lerp(transform->Get_position(), move_pos, Time::deltaTime * 10));
	transform->Set_rotation(transform->LookAt(target));
}
void Camera_Set::Free()
{
	Input_Manager::mouse->SetMode(Mouse::MODE_RELATIVE);

	if (Input_Manager::kb.W)
	{
		move_pos = transform->Get_position() + transform->Get_forward() * 300 * Time::deltaTime;
		transform->Set_position(Vector3::Lerp(transform->Get_position(), move_pos, Time::deltaTime * 10));
	}
	if (Input_Manager::kb.S)
	{
		move_pos = transform->Get_position() - transform->Get_forward() * 300 * Time::deltaTime;
		transform->Set_position(Vector3::Lerp(transform->Get_position(), move_pos, Time::deltaTime * 10));
	}
	if (Input_Manager::kb.D)
	{
		move_pos = transform->Get_position() + transform->Get_right() * 300 * Time::deltaTime;
		transform->Set_position(Vector3::Lerp(transform->Get_position(), move_pos, Time::deltaTime * 10));
	}
	if (Input_Manager::kb.A)
	{
		move_pos = transform->Get_position() - transform->Get_right() * 300 * Time::deltaTime;
		transform->Set_position(Vector3::Lerp(transform->Get_position(), move_pos, Time::deltaTime * 10));
	}

	static Vector3 rot = { 0,0,0 };
	rot.x = Input_Manager::ms.y;
	rot.y = Input_Manager::ms.x;
	transform->Set_eulerAngles(transform->Get_eulerAngles() + rot);
}