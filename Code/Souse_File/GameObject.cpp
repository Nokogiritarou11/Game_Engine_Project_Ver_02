#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "GameObject_Manager.h"
#include "MonoBehaviour_Manager.h"
using namespace std;

GameObject::GameObject()
{
}

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
            Set_OnEnable_OnDisable(com);
        }
        Old_Active = Active;
    }
}

weak_ptr<GameObject> GameObject::Find(string Name)
{
    return GameObject_Manager::Find(Name);
}

weak_ptr<GameObject> GameObject::FindWithTag(string Tag)
{
    return GameObject_Manager::FindWithTag(Tag);
}

void GameObject::Set_OnEnable_OnDisable(std::shared_ptr<Component> comp)
{
    shared_ptr<MonoBehaviour> buff = std::dynamic_pointer_cast<MonoBehaviour>(comp);
    if (buff != nullptr)
    {
        if (Active)
        {
            MonoBehaviour_Manager::Add_Enable(buff);
        }
        else
        {
            MonoBehaviour_Manager::Add_Disable(buff);
        }
    }
    return;
}