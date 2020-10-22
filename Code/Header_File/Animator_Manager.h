#pragma once
#include "Animator.h"

class Animator_Manager
{
public:

	void Reset();

	void Update();
	void Add(std::shared_ptr<Animator> aniamtor);

private:
	std::list<std::weak_ptr<Animator>> Animator_list;
};