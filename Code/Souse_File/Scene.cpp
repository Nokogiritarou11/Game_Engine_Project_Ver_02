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
	obj->ID_Count++;
	obj->ID = obj->ID_Count;
	return obj;
}

void Scene::Destroy_GameObject(shared_ptr<GameObject> gameObject)
{
	list<shared_ptr<GameObject>>::iterator itr_end = gameObject_List.end();
	for (list<shared_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != itr_end; itr++)
	{
		if ((*itr)->ID == gameObject->ID)
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
	for (list<shared_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != itr_end; itr++)
	{
		if ((*itr)->ID == component->gameObject->ID)
		{
			list<shared_ptr<Component>>::iterator itr_comp_end = (*itr)->Component_List.end();
			for (list<shared_ptr<Component>>::iterator itr_comp = (*itr)->Component_List.begin(); itr_comp != itr_comp_end; itr_comp++)
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
	list<shared_ptr<GameObject>>::iterator itr_end = gameObject_List.end();
	for (list<shared_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != itr_end; itr++)
	{
		if ((*itr)->name == Name)
		{
			return *itr;
		}
	}
	weak_ptr<GameObject> n;
	return n;
}

weak_ptr<GameObject> Scene::FindWithTag(std::string Tag)
{
	list<shared_ptr<GameObject>>::iterator itr_end = gameObject_List.end();
	for (list<shared_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != itr_end; itr++)
	{
		if ((*itr)->CompareTag(Tag))
		{
			return *itr;
		}
	}
	weak_ptr<GameObject> n;
	return n;
}

void Scene::Update()
{
	if (!MonoBehaviour_Next_Update_list.empty())
	{
		Update_Stage = 0;
		for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Next_Update_list.begin(); itr != MonoBehaviour_Next_Update_list.end();)
		{
			if (itr->expired())
			{
				itr = MonoBehaviour_Next_Update_list.erase(itr);
				continue;
			}
			else
			{
				shared_ptr<MonoBehaviour> mono = itr->lock();
				MonoBehaviour_Update_list.emplace_back(mono);
				itr = MonoBehaviour_Next_Update_list.erase(itr);
				continue;
			}
		}
	}
	if (!MonoBehaviour_Awake_list.empty())
	{
		Update_Stage = 1;
		for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Awake_list.begin(); itr != MonoBehaviour_Awake_list.end();)
		{
			if (itr->expired())
			{
				itr = MonoBehaviour_Awake_list.erase(itr);
				continue;
			}
			shared_ptr<MonoBehaviour> mono = itr->lock();
			if (mono->gameObject->activeSelf())
			{
				mono->Awake();
				itr = MonoBehaviour_Awake_list.erase(itr);
				continue;
			}
			itr++;
		}
	}
	if (!MonoBehaviour_Enable_list.empty())
	{
		Update_Stage = 2;
		for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Enable_list.begin(); itr != MonoBehaviour_Enable_list.end();)
		{
			if (itr->expired())
			{
				itr = MonoBehaviour_Enable_list.erase(itr);
				continue;
			}
			shared_ptr<MonoBehaviour> mono = itr->lock();
			if (mono->gameObject->activeSelf())
			{
				if (mono->enabled)
				{
					mono->OnEnable();
					itr = MonoBehaviour_Enable_list.erase(itr);
					continue;
				}
			}
			itr++;
		}
	}
	if (!MonoBehaviour_Start_list.empty())
	{
		Update_Stage = 3;
		for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Start_list.begin(); itr != MonoBehaviour_Start_list.end();)
		{
			if (itr->expired())
			{
				itr = MonoBehaviour_Start_list.erase(itr);
				continue;
			}
			shared_ptr<MonoBehaviour> mono = itr->lock();
			if (mono->gameObject->activeSelf())
			{
				if (mono->enabled)
				{
					mono->Start();
					itr = MonoBehaviour_Start_list.erase(itr);
					continue;
				}
			}
			itr++;
		}
	}

	for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Update_list.begin(); itr != MonoBehaviour_Update_list.end();)
	{
		Update_Stage = 4;
		if (itr->expired())
		{
			itr = MonoBehaviour_Update_list.erase(itr);
			continue;
		}
		shared_ptr<MonoBehaviour> mono = itr->lock();
		if (mono->gameObject->activeSelf())
		{
			if (mono->enabled)
			{
				mono->Update();
			}
		}
		itr++;
	}
	for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Update_list.begin(); itr != MonoBehaviour_Update_list.end();)
	{
		Update_Stage = 5;
		if (itr->expired())
		{
			itr = MonoBehaviour_Update_list.erase(itr);
			continue;
		}
		shared_ptr<MonoBehaviour> mono = itr->lock();
		if (mono->gameObject->activeSelf())
		{
			if (mono->enabled)
			{
				mono->LateUpdate();
			}
		}
		itr++;
	}

	if (!MonoBehaviour_Disable_list.empty())
	{
		Update_Stage = 6;
		for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Disable_list.begin(); itr != MonoBehaviour_Disable_list.end();)
		{
			if (itr->expired())
			{
				itr = MonoBehaviour_Disable_list.erase(itr);
				continue;
			}
			shared_ptr<MonoBehaviour> mono = itr->lock();
			if (mono->enabled)
			{
				mono->OnDisable();
				itr = MonoBehaviour_Disable_list.erase(itr);
				continue;
			}
			itr++;
		}
	}
}

void Scene::Reset()
{
	gameObject_List.clear();
	MonoBehaviour_Update_list.clear();
	MonoBehaviour_Awake_list.clear();
	MonoBehaviour_Start_list.clear();
	MonoBehaviour_Disable_list.clear();
	MonoBehaviour_Enable_list.clear();
	Update_Stage = 0;
}

void Scene::Add(shared_ptr<MonoBehaviour> mono)
{
	switch (Update_Stage)
	{
		case 0: //初期
			MonoBehaviour_Awake_list.emplace_back(mono);
			MonoBehaviour_Enable_list.emplace_back(mono);
			MonoBehaviour_Start_list.emplace_back(mono);
			MonoBehaviour_Update_list.emplace_back(mono);
			break;
		case 1: //Awake
			mono->Awake();
			MonoBehaviour_Enable_list.emplace_back(mono);
			MonoBehaviour_Start_list.emplace_back(mono);
			MonoBehaviour_Next_Update_list.emplace_back(mono);
			break;
		case 2: //Enabled
			mono->Awake();
			mono->OnEnable();
			MonoBehaviour_Start_list.emplace_back(mono);
			MonoBehaviour_Next_Update_list.emplace_back(mono);
			break;
		case 3: //Start
			mono->Awake();
			mono->OnEnable();
			MonoBehaviour_Start_list.emplace_back(mono);
			MonoBehaviour_Next_Update_list.emplace_back(mono);
			break;
		case 4: //Update
			mono->Awake();
			mono->OnEnable();
			mono->Start();
			MonoBehaviour_Next_Update_list.emplace_back(mono);
			break;
		case 5: //LateUpdate
			mono->Awake();
			mono->OnEnable();
			mono->Start();
			MonoBehaviour_Next_Update_list.emplace_back(mono);
			break;
		case 6: //Disabled
			mono->Awake();
			mono->OnEnable();
			mono->Start();
			MonoBehaviour_Next_Update_list.emplace_back(mono);
			break;
		default:
			break;
	}
}
void Scene::Add_Disable(shared_ptr<MonoBehaviour> mono)
{
	switch (Update_Stage)
	{
		case 0: //初期
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
		case 0: //初期
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
/////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void Game_01_Scene::Initialize()
{
	shared_ptr<GameObject> camera = GameObject::Instantiate(u8"Main_Camera");
	camera->AddComponent<Camera>();
	camera->AddComponent<Camera_Set>();
	camera->transform->Set_position(0, 75, -125.0f);
	camera->transform->Set_eulerAngles(30, 0, 0);

	shared_ptr<GameObject> player = GameObject::Instantiate(u8"player");
	player->tag = "Player";
	player->AddComponent<Player>();
	shared_ptr<SkinMesh_Renderer> renderer = player->AddComponent<SkinMesh_Renderer>();
	renderer->Set_Mesh(Mesh::Load_Mesh("Model\\", "tank"));
	//renderer->Set_Mesh(Mesh::Load_Mesh("Model\\", "robot_00_01.txt"));

	shared_ptr<GameObject> enemy_manager = GameObject::Instantiate(u8"Enemy_Manager");
	enemy_manager->tag = "Enemy_Manager";
	enemy_manager->AddComponent<Enemy_Manager>();

	shared_ptr<GameObject> Floor = GameObject::Instantiate(u8"Floor");
	shared_ptr<SkinMesh_Renderer> f_renderer = Floor->AddComponent<SkinMesh_Renderer>();
	Floor->transform->Set_eulerAngles(-90, 0, 0);
	Floor->transform->Set_scale(5, 5, 5);
	f_renderer->Set_Mesh(Mesh::Load_Mesh("Model\\", "haikei_yuka1"));
	f_renderer->material[0]->color = { 0,1,0,0 };

	shared_ptr<GameObject> pool = GameObject::Instantiate(u8"ObjectPool");
	pool->tag = "ObjectPool";
	pool->AddComponent<ObjectPool>();
}
*/