#pragma once
#include "doragon.h"
#include "GameObject.h"
#include "Player.h"
class Doragon;
enum ActionSTATE
{
	SUCCESS = 0, // ê¨å˜
	FAILED,	     // é∏îs
	END = 2,     // èIóπ
};
class Action
{
public:
	static bool is_anime;

	Doragon* doragon;
	std::weak_ptr<GameObject> player;

	virtual ActionSTATE run() = 0;
};


class HowlingAction : public Action
{
public:
	static HowlingAction* getInstance(Doragon *doragon)
	{
		static HowlingAction instance;
		instance.player = GameObject::Find("Player").lock();
		instance.doragon = doragon;
		is_anime = false;
		return &instance;
	}
	ActionSTATE run() ;
};

class WalkAction : public Action
{
public:
	static WalkAction* getInstance(Doragon* doragon)
	{
		static WalkAction instance;
		instance.player = GameObject::Find("Player").lock();
		instance.doragon = doragon;
		is_anime = false;

		return &instance;
	}
	ActionSTATE run() ;
};

class MaulAction : public Action
{
public:
	static MaulAction* getInstance(Doragon* doragon)
	{
		static MaulAction instance;
		instance.player = GameObject::Find("Player").lock();
		instance.doragon = doragon;
		is_anime = false;

		return &instance;
	}
	ActionSTATE run() ;
};

class FireballAction : public Action
{
public:
	static FireballAction* getInstance(Doragon* doragon)
	{
		static FireballAction instance;
		instance.player = GameObject::Find("Player").lock();
		instance.doragon = doragon;
		is_anime = false;

		return &instance;
	}
	ActionSTATE run() ;
};

class BlessAction : public Action
{
public:
	static BlessAction* getInstance(Doragon* doragon)
	{
		static BlessAction instance;
		instance.player = GameObject::Find("Player").lock();
		instance.doragon = doragon;
		is_anime = false;

		return &instance;
	}
	ActionSTATE run() ;
};