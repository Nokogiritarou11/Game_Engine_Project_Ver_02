#include "Scene.h"
#include "Mesh_Renderer.h"
#include "SkinMesh_Renderer.h"
#include "Sprite_Renderer.h"
#include "Animator.h"
#include "Transform.h"
#include "GameObject.h"
#include <typeinfo>
using namespace std;

shared_ptr<GameObject> Scene::Instance_GameObject(std::string name)
{
	shared_ptr<GameObject> obj = make_shared<GameObject>();
	obj->AddComponent<Transform>();
	gameObject_List.emplace_back(obj);
	obj->name = name;
	return obj;
}

void Scene::Initialize()
{
	for (shared_ptr<GameObject> g : gameObject_List)
	{
		if (g->transform->Get_parent().expired())
		{
			g->Initialize();
		}
	}
}

void Scene::Destroy_GameObject(shared_ptr<GameObject> gameObject)
{
	gameObject->Release();
	vector<shared_ptr<GameObject>>::iterator itr_end = gameObject_List.end();
	for (vector<shared_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != itr_end; ++itr)
	{
		if ((*itr) == gameObject)
		{
			gameObject_List.erase(itr);
			return;
		}
	}
}
void Scene::Destroy_Component(shared_ptr<Component> component)
{
	vector<shared_ptr<GameObject>>::iterator itr_end = gameObject_List.end();
	for (vector<shared_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != itr_end; ++itr)
	{
		if ((*itr) == component->gameObject)
		{
			vector<shared_ptr<Component>>::iterator itr_comp_end = (*itr)->Component_List.end();
			for (vector<shared_ptr<Component>>::iterator itr_comp = (*itr)->Component_List.begin(); itr_comp != itr_comp_end; ++itr_comp)
			{
				if (typeid(*(*itr_comp)) == typeid(*component))
				{
					(*itr_comp)->gameObject.reset();
					(*itr_comp)->transform.reset();
					(*itr)->Component_List.erase(itr_comp);
					return;
				}
			}
			return;
		}
	}
}

weak_ptr<GameObject> Scene::Find(std::string Name)
{
	for (shared_ptr<GameObject> g : gameObject_List)
	{
		if (g->name == Name)
		{
			return g;
		}
	}
	weak_ptr<GameObject> n;
	return n;
}

weak_ptr<GameObject> Scene::FindWithTag(std::string Tag)
{
	for (shared_ptr<GameObject> g : gameObject_List)
	{
		if (g->CompareTag(Tag))
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
	if (!MonoBehaviour_Start_Next_list.empty())
	{
		copy(MonoBehaviour_Start_Next_list.begin(), MonoBehaviour_Start_Next_list.end(), std::back_inserter(MonoBehaviour_Start_list));
		MonoBehaviour_Start_Next_list.clear();
	}
	if (!MonoBehaviour_Start_list.empty())
	{
		for (weak_ptr<MonoBehaviour> m : MonoBehaviour_Start_list)
		{
			if (!m.expired())
			{
				shared_ptr<MonoBehaviour> mono = m.lock();
				if (mono->gameObject->activeInHierarchy())
				{
					if (mono->enableSelf())
					{
						if (!mono->IsCalled_Start)
						{
							mono->Start();
							mono->IsCalled_Start = true;
							if (mono->gameObject->activeInHierarchy())
							{
								if (mono->enableSelf())
								{
									if (!mono->IsCalled_Update)
									{
										MonoBehaviour_Update_list.emplace_back(m);
										mono->IsCalled_Update = true;
										mono->Disable_flg = false;
									}
								}
							}
						}
					}
				}
			}
		}
		MonoBehaviour_Start_list.clear();
	}
}

void Scene::Processing_Update(int state)
{
	if (!MonoBehaviour_Update_Next_list.empty())
	{
		copy(MonoBehaviour_Update_Next_list.begin(), MonoBehaviour_Update_Next_list.end(), std::back_inserter(MonoBehaviour_Update_list));
		MonoBehaviour_Update_Next_list.clear();
	}

	bool expired = false;
	bool disabled = false;
	for (weak_ptr<MonoBehaviour> m : MonoBehaviour_Update_list)
	{
		if (!m.expired())
		{
			shared_ptr<MonoBehaviour> mono = m.lock();
			if (mono->gameObject->activeInHierarchy())
			{
				if (mono->enableSelf())
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
				else
				{
					mono->Disable_flg = true;
					disabled = true;
				}
			}
			else
			{
				mono->Disable_flg = true;
				disabled = true;
			}
		}
		else
		{
			expired = true;
		}
	}
	if (expired)
	{
		auto removeIt = remove_if(MonoBehaviour_Update_list.begin(), MonoBehaviour_Update_list.end(), [](weak_ptr<MonoBehaviour> m) { return m.expired(); });
		MonoBehaviour_Update_list.erase(removeIt, MonoBehaviour_Update_list.end());
	}
	if (disabled)
	{
		auto removeIt = remove_if(MonoBehaviour_Update_list.begin(), MonoBehaviour_Update_list.end(), [](weak_ptr<MonoBehaviour> m) { shared_ptr<MonoBehaviour> mono = m.lock(); mono->IsCalled_Update = false; return mono->Disable_flg; });
		MonoBehaviour_Update_list.erase(removeIt, MonoBehaviour_Update_list.end());
	}
}

void Scene::Reset()
{
	MonoBehaviour_Update_list.clear();
	MonoBehaviour_Start_list.clear();
	MonoBehaviour_Start_Next_list.clear();

	vector<shared_ptr<GameObject>> no_parent_list;
	for (shared_ptr<GameObject> g : gameObject_List)
	{
		if (g->transform->Get_parent().expired())
		{
			no_parent_list.emplace_back(g);
		}
	}
	for (shared_ptr<GameObject> g : no_parent_list)
	{
		g->Release();
	}
	no_parent_list.clear();
	gameObject_List.clear();
}