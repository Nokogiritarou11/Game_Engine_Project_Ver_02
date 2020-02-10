#pragma once
#include "Animator.h"

class Animator_Manager
{
public:

	static void Reset();

	static void Update();
	static void Add(std::shared_ptr<Animator> aniamtor);

private:
	static std::list<std::weak_ptr<Animator>> Animator_list;

};