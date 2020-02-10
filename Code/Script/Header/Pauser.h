#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
using namespace DirectX;
using namespace std;

class Game_Manager;

class Pauser : public MonoBehaviour
{
public:
	void Start();
	void Update();

	void OnPause();
	void OnResume();
private:
	static list<weak_ptr<Pauser>> targets;
	list<weak_ptr<Behavior>> pauseBehavs;
	weak_ptr<Game_Manager> game_manager;
	bool Pausing;
};