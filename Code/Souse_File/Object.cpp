#include "Object.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene_Manager.h"
using namespace DirectX;
using namespace std;

string Object::ToString()
{
	return name;
}

void Object::Destroy(shared_ptr<GameObject> obj)
{
	Scene_Manager::Get_Active_Scene()->Destroy_GameObject(obj);
}
void Object::Destroy(shared_ptr<Component> comp)
{
	Scene_Manager::Get_Active_Scene()->Destroy_Component(comp);
}

shared_ptr<GameObject> Object::Instantiate(string name)
{
	shared_ptr<GameObject> obj = Scene_Manager::Get_Active_Scene()->Instance_GameObject(name);
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(string name, shared_ptr<Transform> parent)
{
	shared_ptr<GameObject> obj = Scene_Manager::Get_Active_Scene()->Instance_GameObject(name);
	obj->transform->Set_parent(parent);
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(string name, shared_ptr<Transform> parent, bool instantiateInWorldSpace)
{
	shared_ptr<GameObject> obj = Scene_Manager::Get_Active_Scene()->Instance_GameObject(name);
	obj->transform->Set_parent(parent);
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(string name, Vector3 position, Vector3 euler)
{
	shared_ptr<GameObject> obj = Scene_Manager::Get_Active_Scene()->Instance_GameObject(name);
	obj->transform->Set_localPosition(position);
	obj->transform->Set_localEulerAngles(euler);
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(string name, Vector3 position, Vector4 rotation)
{
	shared_ptr<GameObject> obj = Scene_Manager::Get_Active_Scene()->Instance_GameObject(name);
	obj->transform->Set_localPosition(position);
	obj->transform->Set_localRotation(rotation);
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(string name, Vector3 position, Vector3 euler, shared_ptr<Transform> parent)
{
	shared_ptr<GameObject> obj = Scene_Manager::Get_Active_Scene()->Instance_GameObject(name);
	obj->transform->Set_parent(parent);
	obj->transform->Set_localPosition(position);
	obj->transform->Set_localEulerAngles(euler);
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(string name, Vector3 position, Vector4 rotation, shared_ptr<Transform> parent)
{
	shared_ptr<GameObject> obj = Scene_Manager::Get_Active_Scene()->Instance_GameObject(name);
	obj->transform->Set_parent(parent);
	obj->transform->Set_localPosition(position);
	obj->transform->Set_localRotation(rotation);
	return obj;
}