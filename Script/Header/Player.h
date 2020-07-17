#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>
#include "ObjectPool.h"
using namespace DirectX;

class Player : public MonoBehaviour
{
public:

	void Awake();
	void Start();
	void Update();

private:
	std::weak_ptr<ObjectPool> pool;
	Vector3 set_pos = { 0,0,0 };

	float move_speed = 10.0f;
	float rotate_speed = 10.0f;

	//float jump_power = 100.0f;
	//float jump_speed = 0.0;
	float down_speed = 0.0;

	//bool Jumping = false;
};