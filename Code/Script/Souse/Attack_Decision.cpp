#include "Attack_Decision.h"
#include "Game_Manager.h"
#include "Hit_Controller.h"

void Attack_Decision::Start()
{
	shared_ptr<GameObject> obj = GameObject::Find("Game_Manager").lock();
	game_manager = obj->GetComponent<Game_Manager>();
}

void Attack_Decision::Update()
{
	shared_ptr<Game_Manager> game_m = game_manager.lock();
	for (list<weak_ptr<GameObject>>::iterator itr = game_m->Damage_able_List.begin();itr != game_m->Damage_able_List.end();)
	{
		if (itr->expired())
		{
			itr = game_m->Damage_able_List.erase(itr);
			continue;
		}
		shared_ptr<GameObject> obj = itr->lock();
		float dis = Vector3::Distance(transform->position, obj->transform->position);
		if (dis < Size)
		{
			shared_ptr<Hit_Controller> h = obj->GetComponent<Hit_Controller>();
			h->Hit(obj);
			gameObject->SetActive(false);
		}
		itr++;
	}
}