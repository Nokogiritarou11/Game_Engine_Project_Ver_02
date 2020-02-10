#pragma once
#include <stdio.h>
#include <list>
#include <string>
#include <memory>
#include "GameObject.h"

class GameObject_Manager
{
public:

	GameObject_Manager();
	~GameObject_Manager();

	static void Reset();
	static void Desytroy(std::shared_ptr<GameObject> obj);
	static void Update();

	static std::shared_ptr<GameObject> Instance(std::string name);
	static std::weak_ptr<GameObject> Find(std::string Name);
	static std::weak_ptr<GameObject> FindWithTag(std::string Tag);
private:

	static std::list<std::weak_ptr<GameObject>> gameObject_List;
	static unsigned long ID_Count;

};