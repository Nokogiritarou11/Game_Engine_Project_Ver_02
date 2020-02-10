#include "Pauser.h"
#include "Game_Manager.h"

list<weak_ptr<Pauser>> Pauser::targets;

void Pauser::Start()
{
	targets.emplace_back(static_pointer_cast<Pauser>(shared_from_this()));
	shared_ptr<GameObject> obj = GameObject::Find("Game_Manager").lock();
	game_manager = obj->GetComponent<Game_Manager>();
	targets.clear();
}

void Pauser::Update()
{
	shared_ptr<Game_Manager> g = game_manager.lock();
	if (g->Locking && !Pausing)
	{
		OnPause();
		enabled = true;
		Pausing = true;
	}
	if (!g->Locking && Pausing)
	{
		OnResume();
		enabled = true;
		Pausing = false;
	}
}

void Pauser::OnPause()
{
	// 有効なコンポーネントを取得
	for (list<shared_ptr<Component>>::iterator itr = gameObject->Component_List.begin();itr != gameObject->Component_List.end();itr++)
	{
		shared_ptr<Behavior> buff = dynamic_pointer_cast<Behavior>(*itr);
		if (buff != nullptr)
		{
			if (buff->enabled)
			{
				pauseBehavs.emplace_back(buff);
			}
		}
	}

	if (pauseBehavs.empty())
	{
		return;
	}

	for (list<weak_ptr<Behavior>>::iterator itr = pauseBehavs.begin();itr != pauseBehavs.end();itr++)
	{
		shared_ptr<Behavior> b = itr->lock();
		b->enabled = false;
	}
}

void Pauser::OnResume()
{
	if (pauseBehavs.empty())
	{
		return;
	}

	for (list<weak_ptr<Behavior>>::iterator itr = pauseBehavs.begin();itr != pauseBehavs.end();itr++)
	{
		shared_ptr<Behavior> b = itr->lock();
		b->enabled = true;
	}
	pauseBehavs.clear();
}