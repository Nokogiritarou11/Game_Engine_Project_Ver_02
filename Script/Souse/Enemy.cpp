#include "Enemy.h"
#include "Player.h"
#include "SkinMesh_Renderer.h"
#include "Mesh_Renderer.h"
#include "Animator.h"
using namespace std;

void Enemy::Awake()
{
	transform->Set_scale(0.1f, 0.1f, 0.1f);
	shared_ptr<SkinMesh_Renderer> rendrer = GetComponent<SkinMesh_Renderer>();
	rendrer->material[0]->color = { 1,0,0,1 };
}

void Enemy::Start()
{
	weak_ptr<GameObject> g = GameObject::FindWithTag("Player");
	shared_ptr<GameObject> _g = g.lock();
	player = _g->GetComponent<Transform>();
}

void Enemy::Update()
{
	switch (move_state)
	{
		case 0://初期化
			move_state++;
			break;

		case 1://行動判定
			set_move();
			break;
		case 2://待機
			break;
		case 3://旋回
			turn_to_player();
			break;
		case 4://直進
			transform->Set_position(transform->Get_position() + transform->Get_forward() * move_speed * Time::deltaTime);
			break;
		case 5://プレイヤーへ直進
			turn_to_player();
			transform->Set_position(transform->Get_position() + transform->Get_forward() * move_speed * Time::deltaTime);
			break;

		default:
			break;
	}

	move_timer -= Time::deltaTime;
	if (move_timer < 0)
	{
		move_timer = move_timer_set;
		move_state = 1;
	}
}

void Enemy::set_move()
{
	float f = Mathf::Random_Range(0.0f, 100.0f);
	if (f < 25)
	{
		move_state = 2;
	}
	else if (f < 50)
	{
		move_state = 3;
	}
	else if (f < 75)
	{
		move_state = 4;
	}
	else if (f < 100)
	{
		move_state = 5;
	}
}

void Enemy::turn_to_player()
{
	shared_ptr<Transform> p = player.lock();
	Vector3 diff = p->Get_position() - transform->Get_position();
	Vector3 axis = XMVector3Cross(transform->Get_forward(), diff);

	Vector3 rot = transform->Get_eulerAngles();
	if (axis.y > 0.05f)
	{
		rot.y += rotate_speed * Time::deltaTime;
	}
	else if (axis.y < -0.05f)
	{
		rot.y -= rotate_speed * Time::deltaTime;
	}
	transform->Set_eulerAngles(rot);
}