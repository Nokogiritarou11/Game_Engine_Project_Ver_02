#include "GameObject.h"
#include "Scene_Manager.h"
#include "Scene.h"
using namespace std;
using namespace BeastEngine;

void GameObject::Initialize()
{
	for (shared_ptr<Component> c : component_list)
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
			if (shared_ptr<Transform> child = transform->Get_Child(i).lock())
			{
				GameObject::Destroy(child->gameobject);
			}
		}
	}

	for (shared_ptr<Component> c : component_list)
	{
		c->gameobject.reset();
		c->transform.reset();
		c.reset();
	}
	transform.reset();
	component_list.clear();
}

bool GameObject::Compare_Tag(string _tag)
{
	if (tag == _tag)
	{
		return true;
	}
	return false;
}

bool GameObject::Get_Active()
{
	return active;
}

bool GameObject::Get_Active_In_Hierarchy()
{
	if (active)
	{
		if (transform->Get_Parent().expired())
		{
			return active;
		}
		else
		{
			return transform->Get_Parent().lock()->gameobject->Get_Active_In_Hierarchy();
		}
	}
	return active;
}

void GameObject::Set_Active(bool value)
{
	active = value;
	if (active_old != active)
	{
		for (shared_ptr<Component> com : component_list)
		{
			com->Set_Active(active);
		}
		Set_Child_Active(active);
		active_old = active;
	}
}

void GameObject::Set_Child_Active(bool value)
{
	if (transform->Get_Child_Count())
	{
		shared_ptr<Transform> c_trans;
		for (int i = 0; i < transform->Get_Child_Count(); ++i)
		{
			c_trans = transform->Get_Child(i).lock();
			for (shared_ptr<Component> com : c_trans->gameobject->component_list)
			{
				com->Set_Active(value);
			}
			c_trans->gameobject->Set_Child_Active(value);
		}
	}
}

weak_ptr<GameObject> GameObject::Find(string Name)
{
	return Scene_Manager::Get_Active_Scene()->Find(Name);
}

weak_ptr<GameObject> GameObject::Find_With_Tag(string Tag)
{
	return Scene_Manager::Get_Active_Scene()->Find_With_Tag(Tag);
}