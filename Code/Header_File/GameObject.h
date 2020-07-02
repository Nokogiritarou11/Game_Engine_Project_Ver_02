#pragma once
#include <stdio.h>
#include <typeinfo>
#include <string>
#include <list>
#include <memory>
#include "Transform.h"
#include "Mesh_Renderer.h"

class GameObject : public Object
{
public:
	~GameObject();

	bool CompareTag(std::string _tag);
	bool activeSelf();
	void SetActive(bool value);

	unsigned int ID = NULL;
	int layer = 0;
	std::string tag = "Default";
	std::shared_ptr<Transform> transform;
	std::list<std::shared_ptr<Component>> Component_List;

	template<class T>
	std::shared_ptr<T> GetComponent();
	template<class T>
	std::shared_ptr<T> AddComponent();

	static std::weak_ptr<GameObject> Find(std::string Name);
	static std::weak_ptr<GameObject> FindWithTag(std::string Tag);

	static unsigned long ID_Count;
private:
	bool Active = true;
	bool Old_Active = true;

	void Set_OnEnable_OnDisable(std::shared_ptr<Component> comp);
};

template<class T>
std::shared_ptr<T> GameObject::GetComponent()
{
	for (std::shared_ptr<Component> com : Component_List)
	{
		std::shared_ptr<T> buff = std::dynamic_pointer_cast<T>(com);
		if (buff != nullptr)
		{
			if (typeid(shared_ptr<T>) == typeid(buff))
			{
				return buff;
			}
		}
	}
	return nullptr;
}

template<class T>
std::shared_ptr<T> GameObject::AddComponent()
{
	std::shared_ptr<T> buff = std::make_shared<T>();
	buff->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
	Component_List.emplace_back(buff);
	return buff;
}
