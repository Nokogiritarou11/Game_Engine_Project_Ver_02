#include "Scene.h"
#include "Animator.h"
#include "Transform.h"
#include "GameObject.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include <typeinfo>
using namespace std;
using namespace BeastEngine;

shared_ptr<GameObject> Scene::Instance_GameObject(const std::string& name)
{
	shared_ptr<GameObject> obj = make_shared<GameObject>();
	obj->Add_Component<Transform>();
	gameobject_list.emplace_back(obj);
	obj->name = name;
	Engine::asset_manager->Registration_Asset(obj);
	return obj;
}

void Scene::Initialize()
{
	for (const auto& g : gameobject_list)
	{
		if (g->transform->Get_Parent().expired())
		{
			g->Initialize();
		}
	}
}

void Scene::Destroy_GameObject(const shared_ptr<GameObject>& game_object)
{
	game_object->Release();
	const auto itr_end = gameobject_list.end();
	for (auto itr = gameobject_list.begin(); itr != itr_end; ++itr)
	{
		if ((*itr) == game_object)
		{
			gameobject_list.erase(itr);
			return;
		}
	}
}
void Scene::Destroy_Component(const shared_ptr<Component>& component)
{
	const auto itr_end = gameobject_list.end();
	for (auto itr = gameobject_list.begin(); itr != itr_end; ++itr)
	{
		if ((*itr) == component->gameobject)
		{
			const auto itr_comp_end = (*itr)->component_list.end();
			for (auto itr_comp = (*itr)->component_list.begin(); itr_comp != itr_comp_end; ++itr_comp)
			{
				if (typeid(*(*itr_comp)) == typeid(*component))
				{
					(*itr_comp)->gameobject.reset();
					(*itr_comp)->transform.reset();
					(*itr)->component_list.erase(itr_comp);
					return;
				}
			}
			return;
		}
	}
}

weak_ptr<GameObject> Scene::Find(const std::string& name)
{
	for (auto& g : gameobject_list)
	{
		if (g->name == name)
		{
			return g;
		}
	}
	weak_ptr<GameObject> n;
	return n;
}

weak_ptr<GameObject> Scene::Find_With_Tag(const std::string& tag)
{
	for (auto& g : gameobject_list)
	{
		if (g->Compare_Tag(tag))
		{
			return g;
		}
	}
	weak_ptr<GameObject> n;
	return n;
}

void Scene::Update()
{
	Processing_Start();
	Processing_Update(0);
	Processing_Start();
	Processing_Update(1);
}

void Scene::Processing_Start()
{
	if (!monobehaviour_start_next_list.empty())
	{
		copy(monobehaviour_start_next_list.begin(), monobehaviour_start_next_list.end(), std::back_inserter(monobehaviour_start_list));
		monobehaviour_start_next_list.clear();
	}
	if (!monobehaviour_start_list.empty())
	{
		for (const auto& m : monobehaviour_start_list)
		{
			if (const auto& mono = m.lock())
			{
				if (mono->gameobject->Get_Active_In_Hierarchy())
				{
					if (mono->Get_Enabled())
					{
						if (!mono->is_called_start)
						{
							mono->Start();
							mono->is_called_start = true;
							if (mono->gameobject->Get_Active_In_Hierarchy())
							{
								if (mono->Get_Enabled())
								{
									if (!mono->is_called_update)
									{
										monobehaviour_update_list.emplace_back(m);
										mono->is_called_update = true;
									}
								}
							}
						}
					}
				}
			}
		}
		monobehaviour_start_list.clear();
	}
}

void Scene::Processing_Update(int state)
{
	if (!monobehaviour_update_next_list.empty())
	{
		copy(monobehaviour_update_next_list.begin(), monobehaviour_update_next_list.end(), std::back_inserter(monobehaviour_update_list));
		monobehaviour_update_next_list.clear();
	}

	bool expired = false;
	for (const auto& m : monobehaviour_update_list)
	{
		if (const auto& mono = m.lock())
		{
			if (mono->gameobject->Get_Active_In_Hierarchy())
			{
				if (mono->Get_Enabled())
				{
					if (state == 0)
					{
						mono->Update();
					}
					else
					{
						mono->LateUpdate();
					}
				}
			}
		}
		else
		{
			expired = true;
		}
	}
	if (expired)
	{
		const auto& remove_it = remove_if(monobehaviour_update_list.begin(), monobehaviour_update_list.end(), [](weak_ptr<MonoBehaviour> m) { return m.expired(); });
		monobehaviour_update_list.erase(remove_it, monobehaviour_update_list.end());
	}
}

void Scene::Reset()
{
	monobehaviour_update_list.clear();
	monobehaviour_start_list.clear();
	monobehaviour_start_next_list.clear();

	vector<shared_ptr<GameObject>> no_parent_list;
	for (const auto& g : gameobject_list)
	{
		if (g->transform->Get_Parent().expired())
		{
			no_parent_list.emplace_back(g);
		}
	}
	for (const auto& g : no_parent_list)
	{
		g->Release();
	}
	no_parent_list.clear();
	gameobject_list.clear();
}