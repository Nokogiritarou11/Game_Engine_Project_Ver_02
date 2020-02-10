#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

class Game_Manager;
class Animator;

class Player : public MonoBehaviour
{
public:
	int HP = 100;
	bool Hit = false;
	bool Death = false;

	void Awake();
	void Start();
	void Update();

	void Damage();
private:
	float move_speed = 30.0f;
	weak_ptr<Game_Manager> game_manager;
	weak_ptr<Animator> animator;
	int Damage_State = 0;

	enum Anim_index
	{
		Walk,
		Dash,
		Wait,
		Attack_01,
		Attack_02,
		Attack_03,
		Damage_01,
		Damage_02,
		Damage_03,
		Death_anim
	};
};