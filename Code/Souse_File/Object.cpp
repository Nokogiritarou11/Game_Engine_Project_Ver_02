#include "Object.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene_Manager.h"
using namespace DirectX;
using namespace std;

Object::Object()
{
}

Object::~Object()
{
}

string Object::ToString()
{
	return name;
}

void Object::Destroy(shared_ptr<GameObject> obj)
{
	Scene_Manager::Destroy_GameObject(obj);
}

shared_ptr<GameObject> Object::Instantiate(string name)
{
	shared_ptr<GameObject> obj = Scene_Manager::Instance_GameObject(name);
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(string name, shared_ptr<Transform> parent)
{
	shared_ptr<GameObject> obj = Scene_Manager::Instance_GameObject(name);
	obj->transform->parent     = parent;
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(string name, shared_ptr<Transform> parent, bool instantiateInWorldSpace)
{
	shared_ptr<GameObject> obj = Scene_Manager::Instance_GameObject(name);
	obj->transform->parent     = parent;
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(string name, Vector3 position, Vector3 euler)
{
	shared_ptr<GameObject> obj  = Scene_Manager::Instance_GameObject(name);
	obj->transform->position    = position;
	obj->transform->eulerAngles = euler;
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(string name, Vector3 position, Vector4 rotation)
{
	shared_ptr<GameObject> obj = Scene_Manager::Instance_GameObject(name);
	obj->transform->position   = position;
	obj->transform->rotation   = rotation;
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(string name, Vector3 position, Vector3 euler, shared_ptr<Transform> parent)
{
	shared_ptr<GameObject> obj       = Scene_Manager::Instance_GameObject(name);
	obj->transform->parent           = parent;
	obj->transform->localPosition    = position;
	obj->transform->localEulerAngles = euler;
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(string name, Vector3 position, Vector4 rotation, shared_ptr<Transform> parent)
{
	shared_ptr<GameObject> obj    = Scene_Manager::Instance_GameObject(name);
	obj->transform->parent        = parent;
	obj->transform->localPosition = position;
	obj->transform->localRotation = rotation;
	return obj;
}