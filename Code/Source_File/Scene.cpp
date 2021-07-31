#include "Scene.h"
#include "Mesh_Renderer.h"
#include "SkinMesh_Renderer.h"
#include "Sprite_Renderer.h"
#include "Animator.h"
#include "Transform.h"
#include "GameObject.h"
#include "Engine.h"
#include "Shadow_Manager.h"
#include <typeinfo>
using namespace std;
using namespace BeastEngine;

shared_ptr<GameObject> Scene::Instance_GameObject(std::string name)
{
	shared_ptr<GameObject> obj = make_shared<GameObject>();
	obj->Add_Component<Transform>();
	gameobject_list.emplace_back(obj);
	obj->name = name;
	return obj;
}

void Scene::Initialize()
{
	for (auto& g : gameobject_list)
	{
		if (g->transform->Get_Parent().expired())
		{
			g->Initialize();
		}
	}
}

void Scene::Destroy_GameObject(shared_ptr<GameObject> gameObject)
{
	gameObject->Release();
	vector<shared_ptr<GameObject>>::iterator itr_end = gameobject_list.end();
	for (vector<shared_ptr<GameObject>>::iterator itr = gameobject_list.begin(); itr != itr_end; ++itr)
	{
		if ((*itr) == gameObject)
		{
			gameobject_list.erase(itr);
			return;
		}
	}
}
void Scene::Destroy_Component(shared_ptr<Component> component)
{
	vector<shared_ptr<GameObject>>::iterator itr_end = gameobject_list.end();
	for (vector<shared_ptr<GameObject>>::iterator itr = gameobject_list.begin(); itr != itr_end; ++itr)
	{
		if ((*itr) == component->gameobject)
		{
			vector<shared_ptr<Component>>::iterator itr_comp_end = (*itr)->component_list.end();
			for (vector<shared_ptr<Component>>::iterator itr_comp = (*itr)->component_list.begin(); itr_comp != itr_comp_end; ++itr_comp)
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

weak_ptr<GameObject> Scene::Find(std::string Name)
{
	for (auto& g : gameobject_list)
	{
		if (g->name == Name)
		{
			return g;
		}
	}
	weak_ptr<GameObject> n;
	return n;
}

weak_ptr<GameObject> Scene::Find_With_Tag(std::string Tag)
{
	for (auto& g : gameobject_list)
	{
		if (g->Compare_Tag(Tag))
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
	if (!monobehaviour_Start_next_list.empty())
	{
		copy(monobehaviour_Start_next_list.begin(), monobehaviour_Start_next_list.end(), std::back_inserter(monobehaviour_Start_list));
		monobehaviour_Start_next_list.clear();
	}
	if (!monobehaviour_Start_list.empty())
	{
		for (auto& m : monobehaviour_Start_list)
		{
			if (!m.expired())
			{
				shared_ptr<MonoBehaviour> mono = m.lock();
				if (mono->gameobject->Get_Active_In_Hierarchy())
				{
					if (mono->Get_Enabled())
					{
						if (!mono->is_called_Start)
						{
							mono->Start();
							mono->is_called_Start = true;
							if (mono->gameobject->Get_Active_In_Hierarchy())
							{
								if (mono->Get_Enabled())
								{
									if (!mono->is_called_Update)
									{
										monobehaviour_Update_list.emplace_back(m);
										mono->is_called_Update = true;
									}
								}
							}
						}
					}
				}
			}
		}
		monobehaviour_Start_list.clear();
	}
}

void Scene::Processing_Update(int state)
{
	if (!monobehaviour_Update_next_list.empty())
	{
		copy(monobehaviour_Update_next_list.begin(), monobehaviour_Update_next_list.end(), std::back_inserter(monobehaviour_Update_list));
		monobehaviour_Update_next_list.clear();
	}

	bool expired = false;
	for (auto& m : monobehaviour_Update_list)
	{
		if (!m.expired())
		{
			shared_ptr<MonoBehaviour> mono = m.lock();
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
		auto removeIt = remove_if(monobehaviour_Update_list.begin(), monobehaviour_Update_list.end(), [](weak_ptr<MonoBehaviour> m) { return m.expired(); });
		monobehaviour_Update_list.erase(removeIt, monobehaviour_Update_list.end());
	}
}

void Scene::Reset()
{
	monobehaviour_Update_list.clear();
	monobehaviour_Start_list.clear();
	monobehaviour_Start_next_list.clear();

	vector<shared_ptr<GameObject>> no_parent_list;
	for (auto& g : gameobject_list)
	{
		if (g->transform->Get_Parent().expired())
		{
			no_parent_list.emplace_back(g);
		}
	}
	for (auto& g : no_parent_list)
	{
		g->Release();
	}
	no_parent_list.clear();
	gameobject_list.clear();
}