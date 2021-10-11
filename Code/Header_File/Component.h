#pragma once
#include "Object.h"
#include <typeinfo>
#include <memory>

namespace BeastEngine
{
	class Editor;

	class Component : public Object
	{
	public:
		std::shared_ptr<GameObject> gameobject;
		std::shared_ptr<Transform> transform;

		[[nodiscard]] bool Compare_Tag(const std::string& tag) const;

		template<class T>
		std::shared_ptr<T> Get_Component();
		template<class T>
		std::shared_ptr<T> Add_Component();
		std::shared_ptr<Component> Add_Component(const std::string& class_name);

	protected:
		virtual void Set_Active(bool value) {};
		Component() = default;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Object>(this));
		}

		virtual bool Can_Multiple() { return true; };

		friend class GameObject;
		virtual void Initialize(const std::shared_ptr<GameObject>& obj) {};

		friend class Editor;
		virtual bool Draw_ImGui() { return true; };
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Component)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Component)
CEREAL_CLASS_VERSION(BeastEngine::Component, 1)

namespace BeastEngine
{
	namespace Component_Data
	{
		template<class T>
		std::shared_ptr<Component> Create_Component() { return std::make_shared<T>(); }

		struct Component_Factory
		{
			typedef std::map<std::string, std::shared_ptr<Component>(*)()> map_type;

			static std::shared_ptr<Component> createInstance(std::string const& s)
			{
				const auto it = getMap()->find(s);
				if (it == getMap()->end())
					return nullptr;
				return it->second();
			}

			static std::shared_ptr<map_type> getMap()
			{
				if (!map) { map = std::make_shared<map_type>(); }
				return map;
			}

		private:
			static std::shared_ptr<map_type> map;
		};

		template<class T>
		struct Component_Register : Component_Factory
		{
			Component_Register(std::string const& s)
			{
				getMap()->insert(std::make_pair(s, &Create_Component<T>));
			}
		};
	}
}

#define REGISTER_COMPONENT(NAME) \
namespace BeastEngine\
{\
	namespace Component_Data\
	{\
		static BeastEngine::Component_Data::Component_Register<NAME> reg_##NAME(#NAME);\
	}\
}

template<class T>
std::shared_ptr<T> BeastEngine::Component::Get_Component()
{
	for (std::shared_ptr<Component>& com : gameobject->component_list)
	{
		std::shared_ptr<T> buff = std::dynamic_pointer_cast<T>(com);
		if (buff != nullptr)
		{
			return buff;
		}
	}
	return nullptr;
}

template<class T>
std::shared_ptr<T> BeastEngine::Component::Add_Component()
{
	std::shared_ptr<T> buff = std::make_shared<T>();

	if (std::dynamic_pointer_cast<Component>(buff)->Can_Multiple())
	{
		std::dynamic_pointer_cast<Component>(buff)->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
		gameobject->component_list.emplace_back(buff);
		return buff;
	}

	bool already_attach = false;
	for (std::shared_ptr<Component>& com : gameobject->component_list)
	{
		std::shared_ptr<T> _buff = std::dynamic_pointer_cast<T>(com);
		if (_buff != nullptr)
		{
			already_attach = true;
			break;
		}
	}
	if (!already_attach)
	{
		std::dynamic_pointer_cast<Component>(buff)->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
		gameobject->component_list.emplace_back(buff);
		return buff;
	}

	return nullptr;
}