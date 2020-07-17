#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>

#include "Enemy_Manager.h"

using namespace DirectX;

class Bullet : public  MonoBehaviour
{
public:
	void Start();
	void Update();
private:
	float speed = 5;
	float timer = 0;
	std::weak_ptr<Enemy_Manager> e_manager;
};