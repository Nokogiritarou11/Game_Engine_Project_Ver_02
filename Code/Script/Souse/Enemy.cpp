#include "Enemy.h"
#include "Player.h"
#include "Game_Manager.h"
#include "Kunai.h"
#include "SkinMesh_Renderer.h"
#include "Mesh_Renderer.h"
#include "Pauser.h"
#include "Hit_Controller.h"
#include "Animator.h"

void Enemy::Awake()
{
	AddComponent<Hit_Controller>();
	transform->eulerAngles = { -90,0,0 };
	transform->scale = { 0.1f,0.1f ,0.1f };
}

void Enemy::OnEnable()
{
	if (game_manager.expired())
	{
		shared_ptr<GameObject> obj = GameObject::Find("Game_Manager").lock();
		game_manager = obj->GetComponent<Game_Manager>();
	}

	shared_ptr<Game_Manager> manager = game_manager.lock();
	manager->LockOn_able_List.emplace_back(gameObject);
	manager->Damage_able_List.emplace_back(gameObject);
	manager->Enemy_Count++;
}

void Enemy::Start()
{
	player = GameObject::FindWithTag("Player");
	animator = GetComponent<Animator>();
	if (Enemy_State == 0)
	{
		shared_ptr<GameObject> w = GameObject::Instantiate("Kunai");
		w->AddComponent<Kunai>();
		w->AddComponent<Pauser>();
		shared_ptr<SkinMesh_Renderer> k = w->AddComponent<SkinMesh_Renderer>();
		k->Set_Mesh(Mesh::Load_Mesh("Model\\main\\", "kunai.txt"));
		w->SetActive(false);
		Weapon = w;
	}
}

void Enemy::Update()
{
	if (!End)
	{
		shared_ptr<Animator> anim = animator.lock();
		if (!Death)
		{
			if (!Hit)
			{
				switch (Enemy_State)
				{
				case 0:
					Enemy_Default();
					break;
				case 1:
					Enemy_Sword();
					break;
				case 2:
					Enemy_Heavy();
					break;
				}
			}
			else
			{
				if (Damage_State == 2)
				{
					if (anim->Animation_End)
					{
						Damage_State = 0;
						Hit = false;
					}
				}
				if (Damage_State == 1)
				{
					if (anim->Animation_End)
					{
						if (Enemy_State == 0)
						{
							anim->Animation_Index = Damage_01_03;
						}
						if (Enemy_State == 1)
						{

						}
						if (Enemy_State == 2)
						{

						}
						Damage_State++;
					}
				}
				if (Damage_State == 0)
				{
					if (anim->Animation_End)
					{
						if (Enemy_State == 0)
						{
							anim->Animation_Index = Damage_01_02;
						}
						if (Enemy_State == 1)
						{

						}
						if (Enemy_State == 2)
						{

						}
						Damage_State++;
					}
				}
			}
		}
		else
		{
			timer += Time::deltaTime;
			if (timer > Death_timer)
			{
				shared_ptr<Game_Manager> manager = game_manager.lock();
				manager->Enemy_Count--;
				End = true;
				gameObject->SetActive(false);
			}
		}
	}
}

void Enemy::Damage()
{
	Hit = true;
	HP -= 10;
	Attack_State = 0;
	Damage_State = 0;
	Audio_Manager::hit->Play();
	shared_ptr<Animator> anim = animator.lock();
	anim->Animation_Loop = false;
	if (HP > 0)
	{
		if (Enemy_State == 0)
		{
			anim->Animation_Index = Damage_01_01;
		}
		if (Enemy_State == 1)
		{

		}
		if (Enemy_State == 2)
		{

		}
	}
	else
	{
		Audio_Manager::death->Play();
		Death = true;
		if (Enemy_State == 0)
		{
			anim->Animation_Index = Death_01;
		}
		if (Enemy_State == 1)
		{

		}
		if (Enemy_State == 2)
		{

		}
		timer = 0;
	}
}

void Enemy::Enemy_Default()
{
	shared_ptr<GameObject> p = player.lock();
	Vector3 target_pos = p->transform->position;
	shared_ptr<Animator> anim = animator.lock();
	if (Move_State == 0)
	{
		anim->Animation_Loop = true;
		anim->Animation_Index = Wait_01;
		transform->eulerAngles.y = -XMConvertToDegrees(atan2(target_pos.z - transform->position.z, target_pos.x - transform->position.x)) - 90;
		timer += Time::deltaTime;
		if (timer > Attack_timer)
		{
			Move_State++;
			timer = 0;
		}
	}

	if (Move_State == 1)
	{
		anim->Animation_Loop = false;
		if (Attack_State == 0)
		{
			anim->Animation_Index = Attack_01_01;
			if (anim->Animation_End)
			{
				Attack_State++;
			}
		}
		if (Attack_State == 1)
		{
			anim->Animation_Index = Attack_01_02;
			if (anim->Animation_End && anim->Animation_Index != Attack_01_01)
			{
				Audio_Manager::attack->Play();
				shared_ptr<GameObject> w = Weapon.lock();
				Vector3 pos = { 0,0,0 };
				pos.x = transform->position.x + sin(XMConvertToRadians(transform->eulerAngles.y)) * -10;
				pos.z = transform->position.z + cos(XMConvertToRadians(transform->eulerAngles.y)) * -10;
				w->transform->position = pos;
				w->transform->eulerAngles = transform->eulerAngles;
				w->SetActive(true);
				Attack_State++;
			}
		}
		if (Attack_State == 2)
		{
			anim->Animation_Index = Attack_01_03;
			if (anim->Animation_End && anim->Animation_Index != Attack_01_02)
			{
				Move_State = 0;
				Attack_State = 0;
			}
		}
	}
}

void Enemy::Enemy_Sword()
{
	shared_ptr<GameObject> p = player.lock();
	Vector3 target_pos = p->transform->position;
	shared_ptr<Animator> anim = animator.lock();

	if (!Hit)
	{
		if (Move_State == 0)
		{
			transform->eulerAngles.y = -XMConvertToDegrees(atan2(target_pos.z - transform->position.z, target_pos.x - transform->position.x)) - 90;
			transform->position.x -= sin(XMConvertToRadians(transform->eulerAngles.y)) * Attack_speed * Time::deltaTime;
			transform->position.z -= cos(XMConvertToRadians(transform->eulerAngles.y)) * Attack_speed * Time::deltaTime;
			float dis = Vector3::Distance(transform->position, p->transform->position);
			if (dis < 5)
			{
				Move_State++;
			}
		}

		if (Move_State == 1)
		{
			Move_State = 0;
		}
	}
}
void Enemy::Enemy_Heavy()
{
}