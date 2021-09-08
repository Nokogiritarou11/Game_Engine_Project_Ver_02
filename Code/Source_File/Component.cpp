#include "Component.h"
#include "Transform.h"
#include "GameObject.h"
using namespace std;
using namespace BeastEngine;
using namespace Component_Data;

shared_ptr<Component_Factory::map_type> Component_Factory::map;

bool Component::Compare_Tag(string _tag)
{
	if (gameobject->tag == _tag)
	{
		return true;
	}
	return false;
}

shared_ptr<Component> Component::Add_Component(string class_name)
{
	shared_ptr<Component> buff = Component_Factory::createInstance(class_name);
	if (buff)
	{
		bool can_multiple = buff->Can_Multiple();

		if (!can_multiple)
		{
			bool already_attach = false;
			for (shared_ptr<Component>& com : gameobject->component_list)
			{
				if (typeid(*buff) == typeid(*com))
				{
					already_attach = true;
					break;
				}
			}
			if (!already_attach)
			{
				buff->Initialize(static_pointer_cast<GameObject>(shared_from_this()));
				gameobject->component_list.emplace_back(buff);
				return buff;
			}
		}
		else
		{
			buff->Initialize(static_pointer_cast<GameObject>(shared_from_this()));
			gameobject->component_list.emplace_back(buff);
			return buff;
		}
	}
	return nullptr;
}