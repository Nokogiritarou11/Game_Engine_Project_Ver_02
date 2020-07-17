#pragma once

#include "MonoBehaviour.h"
#include "GameObject.h"
#include "Transform.h"
#include "enemy.h"
using namespace DirectX;

class Enemy_Manager : public MonoBehaviour
{
public:

	void Awake();
	void Start();
	void Update();

	void Instance_Enemy(Vector3 pos, Vector3 rot);
	std::vector<std::weak_ptr<GameObject>> enemys = {};
private:

};