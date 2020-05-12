#include "Animator_Manager.h"
#include "GameObject.h"
using namespace std;

list<weak_ptr<Animator>> Animator_Manager::Animator_list;

void Animator_Manager::Reset()
{
	Animator_list.clear();
}

void Animator_Manager::Update()
{
	for (list<weak_ptr<Animator>>::iterator itr = Animator_list.begin(); itr != Animator_list.end();)
	{
		if (itr->expired())
		{
			itr = Animator_list.erase(itr);
			continue;
		}
		shared_ptr<Animator> animator = itr->lock();
		if (animator->gameObject->activeSelf())
		{
			if (animator->enabled)
			{
				animator->Update();
			}
		}
		itr++;
	}
}

void Animator_Manager::Add(shared_ptr<Animator> animator)
{
	Animator_list.emplace_back(animator);
}