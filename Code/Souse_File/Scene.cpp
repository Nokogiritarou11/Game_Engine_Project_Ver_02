#include "Scene.h"
#include "MonoBehaviour_Manager.h"
#include "Mesh_Renderer.h"
#include "SkinMesh_Renderer.h"
#include "Sprite_Renderer.h"
#include "Animator.h"
#include "Transform.h"
#include "GameObject.h"
#include "All_Script.h"
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
			gameObject_List.erase(itr);
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
}

void Scene::Update()
{
	MonoBehaviour_Manager::Update();
	/*
	for (list<shared_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != gameObject_List.end();)
	{
		(*itr)->transform->Update();
		itr++;
	}
	for (list<shared_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != gameObject_List.end();)
	{
		(*itr)->transform->isUpdated = false;
		itr++;
	}
	*/
}

void Scene::Reset()
{
	gameObject_List.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

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
	renderer->Set_Mesh(Mesh::Load_Mesh("Model\\", "tank.txt"));
	//renderer->Set_Mesh(Mesh::Load_Mesh("Model\\", "robot_00_01.txt"));

	shared_ptr<GameObject> enemy_manager = GameObject::Instantiate(u8"Enemy_Manager");
	enemy_manager->tag = "Enemy_Manager";
	enemy_manager->AddComponent<Enemy_Manager>();

	shared_ptr<GameObject> Floor = GameObject::Instantiate(u8"Floor");
	shared_ptr<SkinMesh_Renderer> f_renderer = Floor->AddComponent<SkinMesh_Renderer>();
	Floor->transform->Set_eulerAngles(-90, 0, 0);
	Floor->transform->Set_scale(5, 5, 5);
	f_renderer->Set_Mesh(Mesh::Load_Mesh("Model\\", "haikei_yuka1.txt"));
	f_renderer->material[0]->color = { 0,1,0,0 };

	shared_ptr<GameObject> pool = GameObject::Instantiate(u8"ObjectPool");
	pool->tag = "ObjectPool";
	pool->AddComponent<ObjectPool>();
}