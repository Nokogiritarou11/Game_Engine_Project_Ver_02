#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
using namespace DirectX;
using namespace std;

class Game_Manager;

class Attack_Decision : public MonoBehaviour
{
public:

	void Start();
	void Update();

	int Damage = 100;
	float Size = 1.0f;
private:
	weak_ptr<Game_Manager> game_manager;
};