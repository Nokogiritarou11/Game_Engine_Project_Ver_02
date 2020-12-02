#pragma once
#include "doragon.h"
#include "GameObject.h"
#include "Player.h"

class Doragon;

class ExecJudgment
{
public:
	Doragon* doragon;
	std::weak_ptr<GameObject> player;

	virtual bool judgment() = 0;
};


class AttackJudgment : public ExecJudgment
{
public:
	static AttackJudgment* getInstance(Doragon* doragon)
	{
		static AttackJudgment instance;
		instance.player = GameObject::Find("Player").lock();
		instance.doragon = doragon;
		return &instance;
	}
	bool judgment();
};

class HowlingJudgment : public ExecJudgment
{
public:
	static HowlingJudgment* getInstance(Doragon* doragon)
	{
		static HowlingJudgment instance;
		instance.player = GameObject::Find("Player").lock();
		instance.doragon = doragon;
		return &instance;
	}
	bool judgment();
};


class MaulJudgment : public ExecJudgment
{
public:
	static MaulJudgment* getInstance(Doragon* doragon)
	{
		static MaulJudgment instance;
		instance.player = GameObject::Find("Player").lock();
		instance.doragon = doragon;
		return &instance;
	}
	bool judgment();
};


class FireballJudgment : public ExecJudgment
{
public:
	static FireballJudgment* getInstance(Doragon* doragon)
	{
		static FireballJudgment instance;
		instance.player = GameObject::Find("Player").lock();
		instance.doragon = doragon;
		return &instance;
	}
	bool judgment();
};

class BlessJudgment : public ExecJudgment
{
public:
	static BlessJudgment* getInstance(Doragon* doragon)
	{
		static BlessJudgment instance;
		instance.player = GameObject::Find("Player").lock();
		instance.doragon = doragon;
		return &instance;
	}
	bool judgment();
};

class WalkJudgment : public ExecJudgment
{
public:
	static WalkJudgment* getInstance(Doragon* doragon)
	{
		static WalkJudgment instance;
		instance.player = GameObject::Find("Player").lock();
		instance.doragon = doragon;
		return &instance;
	}
	bool judgment();
};