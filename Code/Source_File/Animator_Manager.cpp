#include "Animator_Manager.h"
#include "GameObject.h"
#include "Animator.h"
using namespace std;
using namespace BeastEngine;

void Animator_Manager::Reset()
{
	animator_list.clear();
}

void Animator_Manager::Update()
{
	for (list<weak_ptr<Animator>>::iterator itr = animator_list.begin(); itr != animator_list.end();)
	{
		if (auto& animator = itr->lock())
		{
			if (animator->gameobject->Get_Active_In_Hierarchy())
			{
				if (animator->Get_Enabled())
				{
					animator->Update();
				}
			}
			++itr;
		}
		else
		{
			itr = animator_list.erase(itr);
		}
	}
}

void Animator_Manager::Add(shared_ptr<Animator> animator)
{
	animator_list.emplace_back(animator);
}