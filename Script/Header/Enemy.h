#pragma once
#include "MonoBehaviour.h"
#include "GameObject.h"
#include "Transform.h"
using namespace DirectX;

class Enemy : public MonoBehaviour
{
public:
	int HP = 1;

	void Awake();
	void Start();
	void Update();

private:

	std::weak_ptr<Transform> player;

	int move_state = 0;

	float move_speed = 10.0f;
	float rotate_speed = 40.0f;

	float move_timer = 3;
	float move_timer_set = 3;

	void set_move();
	void turn_to_player();
};