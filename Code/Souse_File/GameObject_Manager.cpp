#include "GameObject_Manager.h"
#include "GameObject.h"
#include "Scene_Manager.h"
#include "Transform.h"
using namespace std;

std::list<std::weak_ptr<GameObject>> GameObject_Manager::gameObject_List;
unsigned long GameObject_Manager::ID_Count;

GameObject_Manager::GameObject_Manager()
{
	ID_Count = 0;
}

GameObject_Manager::~GameObject_Manager()
{
}

void GameObject_Manager::Reset()
{
	gameObject_List.clear();
}

void GameObject_Manager::Desytroy(shared_ptr<GameObject> obj)
{
	Scene_Manager::Destroy_GameObject(obj);
}

void GameObject_Manager::Update()
{
	for (list<weak_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != gameObject_List.end();)
	{
		if (itr->expired())
		{
			itr = gameObject_List.erase(itr);
			continue;
		}

		shared_ptr<GameObject> obj = itr->lock();
		obj->transform->Update();
		itr++;
	}
}

shared_ptr<GameObject> GameObject_Manager::Instance(std::string name)
{
	shared_ptr<GameObject> obj = make_shared<GameObject>();
	obj->AddComponent<Transform>();
	gameObject_List.emplace_back(obj);
	Scene_Manager::Instance_GameObject(obj);
	obj->name = name;
	obj->ID = ID_Count;
	ID_Count++;
	return obj;
}

weak_ptr<GameObject> GameObject_Manager::Find(std::string Name)
{
	list<weak_ptr<GameObject>>::iterator itr_end = gameObject_List.end();
	for (list<weak_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != itr_end; itr++)
	{
		shared_ptr<GameObject> obj = itr->lock();
		if (obj->name == Name)
		{
			return *itr;
		}
	}
}

weak_ptr<GameObject> GameObject_Manager::FindWithTag(std::string Tag)
{
	list<weak_ptr<GameObject>>::iterator itr_end = gameObject_List.end();
	for (list<weak_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != itr_end; itr++)
	{
		shared_ptr<GameObject> obj = itr->lock();
		if (obj->CompareTag(Tag))
		{
			return *itr;
		}
	}
}