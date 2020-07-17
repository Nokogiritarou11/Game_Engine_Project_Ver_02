#include "GameObject.h"
#include "Scene_Manager.h"
using namespace std;

unsigned long GameObject::ID_Count;

GameObject::~GameObject()
{
	Component_List.clear();
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
		for (shared_ptr<Component> com : Component_List)
		{
			shared_ptr<MonoBehaviour> buff = std::dynamic_pointer_cast<MonoBehaviour>(com);
			if (buff != nullptr)
			{
				if (Active)
				{
					Scene_Manager::Get_Active_Scene()->Add_Enable(buff);
				}
				else
				{
					Scene_Manager::Get_Active_Scene()->Add_Disable(buff);
				}
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