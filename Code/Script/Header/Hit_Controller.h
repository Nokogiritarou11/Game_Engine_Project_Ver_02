#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
using namespace DirectX;
using namespace std;

class Animator;
class Player;
class Enemy;

class Hit_Controller : public MonoBehaviour
{
public:
	void Start();
	void Hit(shared_ptr<GameObject> obj);
private:
	weak_ptr<Animator> animator;
	weak_ptr<Player> player;
	weak_ptr<Enemy> enemy;
	bool IsEnemy = false;
};