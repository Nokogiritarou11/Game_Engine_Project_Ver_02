#include "GameObject.h"
#include "Scene_Manager.h"
using namespace std;

GameObject::~GameObject()
{
	Component_List.clear();
}

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

void GameObject::SetActive(bool value)
{
	Active = value;
	if (Old_Active != Active)
	{
		shared_ptr<MonoBehaviour> mono;
		for (shared_ptr<Component> com : Component_List)
		{
			mono = std::dynamic_pointer_cast<MonoBehaviour>(com);
			if (mono != nullptr)
			{
				if (Active)
				{
					mono->OnEnable();
				}
				else
				{
					mono->OnDisable();
				}
				continue;
			}
			if (Active)
			{

			}
		}
		Old_Active = Active;
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