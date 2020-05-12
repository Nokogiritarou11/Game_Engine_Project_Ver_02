#include "Player.h"
#include "Game_Manager.h"
#include "Hit_Controller.h"
#include "Animator.h"
#include "Camera.h"
#include "Scene_Manager.h"

void Player::Awake()
{
	transform->eulerAngles = { -90,0,0 };
	transform->scale = { 0.1f,0.1f ,0.1f };
	AddComponent<Hit_Controller>();
}

void Player::Start()
{
	shared_ptr<GameObject> obj = GameObject::Find("Game_Manager").lock();
	game_manager = obj->GetComponent<Game_Manager>();
	shared_ptr<Game_Manager> game_m = game_manager.lock();
	game_m->Damage_able_List.emplace_back(gameObject);
	animator = GetComponent<Animator>();
}

void Player::Update()
{
	shared_ptr<Animator> anim = animator.lock();
	shared_ptr<Game_Manager> game_m = game_manager.lock();
	if (!Death)
	{
		if (!Hit)
		{
			if (!game_m->Locking)
			{
				anim->Animation_Loop = true;
				float Lposx = Input_Manager::pad.thumbSticks.leftX;
				float Lposy = Input_Manager::pad.thumbSticks.leftY;

				if (Lposx != 0 || Lposy != 0)
				{
					anim->Animation_Index = Dash;
					transform->eulerAngles = Vector3(-90, -XMConvertToDegrees(atan2(Lposy, Lposx)) - 90, 0);
					transform->position.x += Lposx * move_speed * Time::deltaTime;
					transform->position.z += Lposy * move_speed * Time::deltaTime;
					transform->position.x = min(max(-90, transform->position.x), 90);
					transform->position.z = min(max(-45, transform->position.z), 45);
				}
				else
				{
					anim->Animation_Index = Wait;
				}
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
					anim->Animation_Index = Damage_03;
					Damage_State++;
				}
			}
			if (Damage_State == 0)
			{
				if (anim->Animation_End)
				{
					anim->Animation_Index = Damage_02;
					Damage_State++;
				}
			}
		}
	}
	else
	{
		game_m->Game_Over = true;
	}
	if (Input_Manager::pad_tracker.start == GamePad::ButtonStateTracker::PRESSED)
	{
		Scene_Manager::LoadScene("Title_Scene");
	}
}

void Player::Damage()
{
	Hit = true;
	HP -= 10;
	Audio_Manager::hit->Play();
	shared_ptr<Animator> anim = animator.lock();
	anim->Animation_Loop = false;
	if (HP > 0)
	{
		anim->Animation_Index = Damage_01;
	}
	else
	{
		Audio_Manager::death->Play();
		Death = true;
		anim->Animation_Index = Death_anim;
	}
}