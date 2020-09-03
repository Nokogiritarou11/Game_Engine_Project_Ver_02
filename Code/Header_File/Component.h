#pragma once
#include "Object.h"
#include <list>
#include <typeinfo>
#include <memory>

class Component : public Object
{
public:
	std::shared_ptr<GameObject> gameObject;
	std::shared_ptr<Transform> transform;
	std::string tag;

	Component();
	~Component();

	bool CompareTag(std::string _tag);

	virtual void Initialize(){};
	virtual void Initialize(std::shared_ptr<GameObject> obj){};
	virtual bool Draw_ImGui(){return true;};
	template<class T>
	std::shared_ptr<T> GetComponent();
	template<class T>
	std::shared_ptr<T> AddComponent();


private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Object>(this), tag);
	}
};

CEREAL_REGISTER_TYPE(Component)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, Component)

template<class T>
std::shared_ptr<T> Component::GetComponent()
{
	for (std::shared_ptr<Component> com : gameObject->Component_List)
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
std::shared_ptr<T> Component::AddComponent()
{
	std::shared_ptr<T> buff = std::make_shared<T>();
	buff->Initialize(gameObject);
	gameObject->Component_List.emplace_back(buff);
	return buff;
}