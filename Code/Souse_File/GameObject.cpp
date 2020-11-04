#include "GameObject.h"
#include "Scene_Manager.h"
using namespace std;

void GameObject::Initialize()
{
	for (shared_ptr<Component> c : Component_List)
	{
		c->Initialize(dynamic_pointer_cast<GameObject>(shared_from_this()));
	}
}

bool GameObject::CompareTag(string _tag)
{
	if (tag == _tag)
	{
		return true;
	}
	return false;
}

bool GameObject::activeSelf()
{
	return Active;
}

bool GameObject::activeInHierarchy()
{
	if (Active)
	{
		if (transform->Get_parent().expired())
		{
			return Active;
		}
		else
		{
			return transform->Get_parent().lock()->gameObject->activeInHierarchy();
		}
	}
	return Active;
}

void GameObject::SetActive(bool value)
{
	Active = value;
	if (Old_Active != Active)
	{
		for (shared_ptr<Component> com : Component_List)
		{
			com->SetActive(Active);
		}
		Set_Child_Active(Active);
		Old_Active = Active;
	}
}

void GameObject::Set_Child_Active(bool value)
{
	if (transform->childCount())
	{
		shared_ptr<Transform> c_trans;
		for (int i = 0; i < transform->childCount(); ++i)
		{
			c_trans = transform->GetChild(i).lock();
			for (shared_ptr<Component> com : c_trans->gameObject->Component_List)
			{
				com->SetActive(value);
			}
			c_trans->gameObject->Set_Child_Active(value);
		}
	}
}

weak_ptr<GameObject> GameObject::Find(string Name)
{
	return Scene_Manager::Get_Active_Scene()->Find(Name);
}

weak_ptr<GameObject> GameObject::FindWithTag(string Tag)
{
	return Scene_Manager::Get_Active_Scene()->FindWithTag(Tag);
}