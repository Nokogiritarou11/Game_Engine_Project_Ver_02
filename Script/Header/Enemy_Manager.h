#pragma once

#include "MonoBehaviour.h"
#include "GameObject.h"
#include "Transform.h"
#include "enemy.h"
using namespace DirectX;
using namespace std;

class Enemy_Manager : public MonoBehaviour
{
public:

	void Awake();
	void Start();
	void Update();

	void Instance_Enemy(Vector3 pos, Vector3 rot);
	vector<weak_ptr<GameObject>> enemys = {};
private:

};