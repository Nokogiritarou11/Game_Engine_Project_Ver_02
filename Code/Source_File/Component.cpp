#include "Component.h"
#include "Transform.h"
#include "GameObject.h"
using namespace std;
using namespace BeastEngine;
using namespace Component_Data;

std::shared_ptr<Component_Factory::map_type> Component_Factory::map;

bool Component::Compare_Tag(string _tag)
{
	if (gameobject->tag == _tag)
	{
		return true;
	}
	return false;
}

std::shared_ptr<Component> Component::Add_Component(std::string class_name)
{
	std::shared_ptr<Component> buff = Component_Factory::createInstance(class_name);
	if (buff)
	{
		bool can_multiple = buff->Can_Multiple();

		if (!can_multiple)
		{
			bool already_attach = false;
			for (std::shared_ptr<Component>& com : gameobject->component_list)
			{
				if (typeid(*buff) == typeid(*com))
				{
					already_attach = true;
				}
			}
			if (!already_attach)
			{
				buff->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
				gameobject->component_list.emplace_back(buff);
				return buff;
			}
		}
		else
		{
			buff->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
			gameobject->component_list.emplace_back(buff);
			return buff;
		}
	}
	return nullptr;
}