#include "GameObject.h"
#include "Scene_Manager.h"
#include "Scene.h"
#include "Engine.h"
#include "Asset_Manager.h"
using namespace std;
using namespace BeastEngine;

std::shared_ptr<Component> GameObject::Add_Component(const std::string& class_name)
{
	if (std::shared_ptr<Component> buff = Component_Data::Component_Factory::Create_Instance(class_name))
	{
		if (buff->Can_Multiple())
		{
			buff->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
			component_list.emplace_back(buff);
			return buff;
		}

		bool already_attach = false;
		for (std::shared_ptr<Component>& com : component_list)
		{
			if (typeid(*buff) == typeid(*com))
			{
				already_attach = true;
			}
		}
		if (!already_attach)
		{
			buff->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
			component_list.emplace_back(buff);
			return buff;
		}
	}
	return nullptr;
}

void GameObject::Initialize()
{
	Engine::asset_manager->Registration_Asset(shared_from_this());
	for (const auto& c : component_list)
	{
		c->Initialize(dynamic_pointer_cast<GameObject>(shared_from_this()));
	}
	for (int i = 0; i < transform->Get_Child_Count(); ++i)
	{
		transform->Get_Child(i).lock()->gameobject->Initialize();
	}
}

void GameObject::Release()
{
	transform->Set_Parent(nullptr);
	while (transform->Get_Child_Count())
	{
		for (int i = 0; i < transform->Get_Child_Count(); ++i)
		{
			if (const auto& child = transform->Get_Child(i).lock())
			{
				Destroy(child->gameobject);
			}
		}
	}

	for (auto& c : component_list)
	{
		c->gameobject.reset();
		c->transform.reset();
		c.reset();
	}
	transform.reset();
	component_list.clear();
}

bool GameObject::Compare_Tag(const string& tag) const
{
	if (this->tag == tag)
	{
		return true;
	}
	return false;
}

bool GameObject::Get_Active() const
{
	return active;
}

bool GameObject::Get_Active_In_Hierarchy() const
{
	if (active)
	{
		if (const auto& obj = transform->Get_Parent().lock())
		{
			return obj->gameobject->Get_Active_In_Hierarchy();
		}
		else
		{
			return active;
		}
	}
	return active;
}

void GameObject::Set_Active(const bool value)
{
	active = value;
	if (active_old != active)
	{
		for (const auto& com : component_list)
		{
			com->Set_Active(active);
		}
		Set_Child_Active(active);
		active_old = active;
	}
}

void GameObject::Set_Child_Active(const bool value) const
{
	if (transform->Get_Child_Count())
	{
		for (int i = 0; i < transform->Get_Child_Count(); ++i)
		{
			const auto& c_trans = transform->Get_Child(i).lock();
			for (const auto& com : c_trans->gameobject->component_list)
			{
				com->Set_Active(value);
			}
			c_trans->gameobject->Set_Child_Active(value);
		}
	}
}

weak_ptr<GameObject> GameObject::Find(const string& name)
{
	return Engine::scene_manager->Get_Active_Scene()->Find(name);
}

weak_ptr<GameObject> GameObject::Find_With_Tag(const string& tag)
{
	return Engine::scene_manager->Get_Active_Scene()->Find_With_Tag(tag);
}