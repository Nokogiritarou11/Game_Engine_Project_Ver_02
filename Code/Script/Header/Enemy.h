#pragma once
#include "MonoBehaviour.h"
#include "GameObject.h"
#include "Transform.h"
using namespace DirectX;
using namespace std;

class Player;
class Game_Manager;
class Animator;

class Enemy : public MonoBehaviour
{
public:
	int HP = 20;
	int Enemy_State = 0;
	bool Hit = false;
	bool Death = false;
	bool End = false;

	void Awake();
	void OnEnable();
	void Start();
	void Update();

	void Damage();

	void Enemy_Default();

private:
	weak_ptr<GameObject> player;
	weak_ptr<GameObject> Weapon;
	weak_ptr<Game_Manager> game_manager;
	weak_ptr<Animator> animator;

	int Move_State = 0;
	int Attack_State = 0;
	int Damage_State = 0;
	float timer = 0;
	float Attack_timer = 3;
	float Attack_speed = 10;
	float Death_timer = 3;

	enum Anim_index_01
	{
		Walk_01,
		Dash_01,
		Wait_01,
		Attack_01_01,
		Attack_01_02,
		Attack_01_03,
		Damage_01_01,
		Damage_01_02,
		Damage_01_03,
		Death_01
	};
};