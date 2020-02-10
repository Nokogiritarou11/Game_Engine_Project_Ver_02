#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
using namespace DirectX;
using namespace std;

class Game_Manager;

class Kunai : public MonoBehaviour
{
public:
	void Start();
	void Update();
private:
	weak_ptr<GameObject> player;
	weak_ptr<Game_Manager> game_manager;
	float speed = 100;
	float timer = 0;
	float Disable_time = 2;
};