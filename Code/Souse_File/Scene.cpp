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
		g->Initialize();
	}
}

void Scene::Destroy_GameObject(shared_ptr<GameObject> gameObject)
{
	list<shared_ptr<GameObject>>::iterator itr_end = gameObject_List.end();
	for (list<shared_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != itr_end; ++itr)
	{
		if ((*itr) == gameObject)
		{
			(*itr)->Component_List.clear();
			gameObject_List.erase(itr);
			return;
		}
	}
}
void Scene::Destroy_Component(shared_ptr<Component> component)
{
	list<shared_ptr<GameObject>>::iterator itr_end = gameObject_List.end();
	for (list<shared_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != itr_end; ++itr)
	{
		if ((*itr) == component->gameObject)
		{
			vector<shared_ptr<Component>>::iterator itr_comp_end = (*itr)->Component_List.end();
			for (vector<shared_ptr<Component>>::iterator itr_comp = (*itr)->Component_List.begin(); itr_comp != itr_comp_end; ++itr_comp)
			{
				if (typeid(*(*itr_comp)) == typeid(*component))
				{
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
	if (!MonoBehaviour_Start_Next_list.empty())
	{
		std::copy(MonoBehaviour_Start_Next_list.begin(), MonoBehaviour_Start_Next_list.end(), std::back_inserter(MonoBehaviour_Start_list));
		MonoBehaviour_Start_Next_list.clear();
	}
	if (!MonoBehaviour_Update_Next_list.empty())
	{
		std::copy(MonoBehaviour_Update_Next_list.begin(), MonoBehaviour_Update_Next_list.end(), std::back_inserter(MonoBehaviour_Update_list));
		MonoBehaviour_Update_Next_list.clear();
	}

	Update_Stage = 1;
	if (!MonoBehaviour_Awake_list.empty())
	{
		for (weak_ptr<MonoBehaviour> m : MonoBehaviour_Awake_list)
		{
			if (!m.expired())
			{
				shared_ptr<MonoBehaviour> mono = m.lock();
				if (mono->gameObject->activeSelf())
				{
					mono->Awake();
					mono->IsCalled_Awake = true;
					if (mono->gameObject->activeSelf())
					{
						MonoBehaviour_Enable_list.emplace_back(m);
					}
				}
			}
		}
		//auto removeIt = remove_if(MonoBehaviour_Awake_list.begin(), MonoBehaviour_Awake_list.end(), [](weak_ptr<MonoBehaviour> m) { shared_ptr<MonoBehaviour> _m = m.lock(); return _m->IsCalled_Awake; });
		//MonoBehaviour_Awake_list.erase(removeIt, MonoBehaviour_Awake_list.end());
		MonoBehaviour_Awake_list.clear();
	}

	Update_Stage = 2;
	if (!MonoBehaviour_Enable_list.empty())
	{
		for (weak_ptr<MonoBehaviour> m : MonoBehaviour_Enable_list)
		{
			if (!m.expired())
			{
				shared_ptr<MonoBehaviour> mono = m.lock();
				mono->OnEnable();
				if (!mono->IsCalled_Start)
				{
					MonoBehaviour_Start_list.emplace_back(m);
				}
			}
		}
		MonoBehaviour_Enable_list.clear();
	}

	Update_Stage = 3;
	if (!MonoBehaviour_Start_list.empty())
	{
		for (weak_ptr<MonoBehaviour> m : MonoBehaviour_Start_list)
		{
			if (!m.expired())
			{
				shared_ptr<MonoBehaviour> mono = m.lock();
				if (mono->gameObject->activeSelf())
				{
					if (mono->enableSelf())
					{
						if (!mono->IsCalled_Start)
						{
							mono->Start();
							mono->IsCalled_Start = true;
							if (mono->gameObject->activeSelf())
							{
								MonoBehaviour_Update_list.emplace_back(m);
							}
						}
					}
				}
			}
		}
		//auto removeIt = remove_if(MonoBehaviour_Start_list.begin(), MonoBehaviour_Start_list.end(), [](weak_ptr<MonoBehaviour> m) { shared_ptr<MonoBehaviour> _m = m.lock(); return _m->IsCalled_Start; });
		//MonoBehaviour_Start_list.erase(removeIt, MonoBehaviour_Start_list.end());
		MonoBehaviour_Start_list.clear();
	}

	Update_Stage = 4;
	bool expired = false;
	bool Disabled = false;
	for (weak_ptr<MonoBehaviour> m : MonoBehaviour_Update_list)
	{
		if (!m.expired())
		{
			shared_ptr<MonoBehaviour> mono = m.lock();
			if (mono->gameObject->activeSelf())
			{
				if (mono->enableSelf())
				{
					mono->Update();
				}
			}
			else
			{
				Disabled = true;
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

	Update_Stage = 5;
	for (weak_ptr<MonoBehaviour> m : MonoBehaviour_Update_list)
	{
		if (!m.expired())
		{
			shared_ptr<MonoBehaviour> mono = m.lock();
			if (mono->gameObject->activeSelf())
			{
				if (mono->enableSelf())
				{
					if (mono->IsCalled_Start)
					{
						mono->LateUpdate();
					}
				}
			}
			else
			{
				Disabled = true;
			}
		}
	}
	if (Disabled)
	{
		auto removeIt = remove_if(MonoBehaviour_Update_list.begin(), MonoBehaviour_Update_list.end(), [](weak_ptr<MonoBehaviour> m) { shared_ptr<MonoBehaviour> mono = m.lock(); return mono->gameObject->activeSelf(); });
		MonoBehaviour_Update_list.erase(removeIt, MonoBehaviour_Update_list.end());
	}

	Update_Stage = 6;
	if (!MonoBehaviour_Disable_list.empty())
	{
		for (weak_ptr<MonoBehaviour> m : MonoBehaviour_Disable_list)
		{
			if (!m.expired())
			{
				shared_ptr<MonoBehaviour> mono = m.lock();
				mono->OnDisable();
			}
		}
		MonoBehaviour_Disable_list.clear();
	}
}

void Scene::Reset()
{
	gameObject_List.clear();
	MonoBehaviour_Update_list.clear();
	MonoBehaviour_Start_list.clear();
	MonoBehaviour_Start_list.clear();
	MonoBehaviour_Disable_list.clear();
	MonoBehaviour_Enable_list.clear();
	Update_Stage = 0;
}

void Scene::Add(shared_ptr<MonoBehaviour> mono)
{
	switch (Update_Stage)
	{
		case 0: //‰Šú
			MonoBehaviour_Awake_list.emplace_back(mono);
			break;
		case 1: //Awake
			if (mono->gameObject->activeSelf())
			{
				mono->Awake();
				if (mono->gameObject->activeSelf())
				{
					MonoBehaviour_Enable_list.emplace_back(mono);
				}
			}
			break;
		case 2: //Enabled
			if (mono->gameObject->activeSelf())
			{
				mono->Awake();
				if (mono->gameObject->activeSelf())
				{
					MonoBehaviour_Enable_list.emplace_back(mono);
				}
			}
			break;
		case 3: //Start
			if (mono->gameObject->activeSelf())
			{
				mono->Awake();
				if (mono->gameObject->activeSelf())
				{
					MonoBehaviour_Enable_list.emplace_back(mono);
				}
			}
			break;
		case 4: //Update
			if (mono->gameObject->activeSelf())
			{
				mono->Awake();
				if (mono->gameObject->activeSelf())
				{
					MonoBehaviour_Enable_list.emplace_back(mono);
				}
			}
			break;
		case 5: //LateUpdate
			if (mono->gameObject->activeSelf())
			{
				mono->Awake();
				if (mono->gameObject->activeSelf())
				{
					MonoBehaviour_Enable_list.emplace_back(mono);
				}
			}
			break;
		case 6: //Disabled
			if (mono->gameObject->activeSelf())
			{
				mono->Awake();
				if (mono->gameObject->activeSelf())
				{
					MonoBehaviour_Enable_list.emplace_back(mono);
				}
			}
			break;
		default:
			break;
	}
}
void Scene::Add_Disable(shared_ptr<MonoBehaviour> mono)
{
	switch (Update_Stage)
	{
		case 0: //‰Šú
			MonoBehaviour_Disable_list.emplace_back(mono);
			break;
		case 1: //Awake
			MonoBehaviour_Disable_list.emplace_back(mono);
			break;
		case 2: //Enabled
			MonoBehaviour_Disable_list.emplace_back(mono);
			break;
		case 3: //Start
			MonoBehaviour_Disable_list.emplace_back(mono);
			break;
		case 4: //Update
			MonoBehaviour_Disable_list.emplace_back(mono);
			break;
		case 5: //LateUpdate
			MonoBehaviour_Disable_list.emplace_back(mono);
			break;
		case 6: //Disabled
			mono->OnDisable();
			break;
		default:
			break;
	}
}
void Scene::Add_Enable(shared_ptr<MonoBehaviour> mono)
{
	switch (Update_Stage)
	{
		case 0: //‰Šú
			MonoBehaviour_Enable_list.emplace_back(mono);
			break;
		case 1: //Awake
			MonoBehaviour_Enable_list.emplace_back(mono);
			break;
		case 2: //Enabled
			mono->OnEnable();
			break;
		case 3: //Start
			mono->OnEnable();
			break;
		case 4: //Update
			mono->OnEnable();
			break;
		case 5: //LateUpdate
			mono->OnEnable();
			break;
		case 6: //Disabled
			mono->OnEnable();
			break;
		default:
			break;
	}
}