#include "Object.h"
#include "GameObject_Manager.h"
#include "GameObject.h"
#include "Transform.h"
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
	GameObject_Manager::Desytroy(obj);
}

shared_ptr<GameObject> Object::Instantiate(std::string name)
{
	shared_ptr<GameObject> obj = GameObject_Manager::Instance(name);
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(std::string name, shared_ptr<Transform> parent)
{
	shared_ptr<GameObject> obj = GameObject_Manager::Instance(name);
	obj->transform->parent = parent;
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(std::string name, shared_ptr<Transform> parent, bool instantiateInWorldSpace)
{
	shared_ptr<GameObject> obj = GameObject_Manager::Instance(name);
	obj->transform->parent = parent;
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(std::string name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 euler)
{
	shared_ptr<GameObject> obj = GameObject_Manager::Instance(name);
	obj->transform->position = position;
	obj->transform->eulerAngles = euler;
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(std::string name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation)
{
	shared_ptr<GameObject> obj = GameObject_Manager::Instance(name);
	obj->transform->position = position;
	obj->transform->rotation = rotation;
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(std::string name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 euler, shared_ptr<Transform> parent)
{
	shared_ptr<GameObject> obj = GameObject_Manager::Instance(name);
	obj->transform->parent = parent;
	obj->transform->localPosition = position;
	obj->transform->localEulerAngles = euler;
	return obj;
}

shared_ptr<GameObject> Object::Instantiate(std::string name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation, shared_ptr<Transform> parent)
{
	shared_ptr<GameObject> obj = GameObject_Manager::Instance(name);
	obj->transform->parent = parent;
	obj->transform->localPosition = position;
	obj->transform->localRotation = rotation;
	return obj;
}