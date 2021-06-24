#pragma once
#include "Object.h"
#include <list>
#include <typeinfo>
#include <memory>

namespace BeastEngine
{
	class Editor;

	class Component : public BeastEngine::Object
	{
	public:
		std::shared_ptr<BeastEngine::GameObject> gameobject;
		std::shared_ptr<BeastEngine::Transform> transform;

		bool Compare_Tag(std::string _tag);

		template<class T>
		std::shared_ptr<T> Get_Component();
		template<class T>
		std::shared_ptr<T> Add_Component();

	protected:
		virtual void Set_Active(bool value) {};
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Object>(this));
		}

		virtual bool Can_Multiple() { return true; };

		friend class BeastEngine::GameObject;
		virtual void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) {};

		friend class BeastEngine::Editor;
		virtual bool Draw_ImGui() { return true; };
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Component)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Component)

template<class T>
std::shared_ptr<T> BeastEngine::Component::Get_Component()
{
	for (std::shared_ptr<BeastEngine::Component> com : gameobject->component_list)
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
std::shared_ptr<T> BeastEngine::Component::Add_Component()
{
	std::shared_ptr<T> buff = std::make_shared<T>();
	bool can_multiple = std::dynamic_pointer_cast<BeastEngine::Component>(buff)->Can_Multiple();

	if (!can_multiple)
	{
		bool already_attach = false;
		for (std::shared_ptr<BeastEngine::Component> com : gameobject->component_list)
		{
			std::shared_ptr<T> _buff = std::dynamic_pointer_cast<T>(com);
			if (_buff != nullptr)
			{
				if (typeid(shared_ptr<T>) == typeid(_buff))
				{
					already_attach = true;
				}
			}
		}
		if (!already_attach)
		{
			std::dynamic_pointer_cast<BeastEngine::Component>(buff)->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
			component_list.emplace_back(buff);
			return buff;
		}
	}
	else
	{
		std::dynamic_pointer_cast<BeastEngine::Component>(buff)->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
		component_list.emplace_back(buff);
		return buff;
	}
	return nullptr;
}