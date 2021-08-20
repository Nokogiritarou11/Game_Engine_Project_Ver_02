#include "Object.h"
#include "GameObject.h"
#include "Transform.h"
#include "Scene_Manager.h"
#include "Scene.h"
#include "System_Function.h"
#include "Engine.h"
#include "Asset_Manager.h"
using namespace DirectX;
using namespace std;
using namespace BeastEngine;

Object::Object()
{
	instance_id = System_Function::Create_ID();
}

Object::Object(const Object& obj)
{
	instance_id = System_Function::Create_ID();
}

Object::~Object()
{
	Engine::asset_manager->Erase_Asset(instance_id);
}

void Object::Destroy(shared_ptr<GameObject> obj)
{
	Engine::scene_manager->Get_Active_Scene()->Destroy_GameObject(obj);
}
void Object::Destroy(shared_ptr<Component> comp)
{
	Engine::scene_manager->Get_Active_Scene()->Destroy_Component(comp);
}

shared_ptr<Object> Object::Instantiate(std::weak_ptr<BeastEngine::Object> original)
{
	shared_ptr<Object> obj = make_shared<Object>(*original.lock());
	return obj;
}
/*
shared_ptr<Object> Object::Instantiate(std::weak_ptr<BeastEngine::Object> original, shared_ptr<Transform> parent)
{
	shared_ptr<Object> obj = make_shared<Object>(*original.lock());
	obj->transform->Set_parent(parent);
	return obj;
}

shared_ptr<Object> Object::Instantiate(std::weak_ptr<BeastEngine::Object> original, shared_ptr<Transform> parent, bool instantiateInWorldSpace)
{
	shared_ptr<Object> obj = make_shared<Object>(*original.lock());
	obj->transform->Set_parent(parent);
	return obj;
}

shared_ptr<Object> Object::Instantiate(std::weak_ptr<BeastEngine::Object> original, Vector3 position, Vector3 euler)
{
	shared_ptr<Object> obj = make_shared<Object>(*original.lock());
	obj->transform->Set_localPosition(position);
	obj->transform->Set_localEulerAngles(euler);
	return obj;
}

shared_ptr<Object> Object::Instantiate(std::weak_ptr<BeastEngine::Object> original, Vector3 position, Vector4 rotation)
{
	shared_ptr<Object> obj = make_shared<Object>(*original.lock());
	obj->transform->Set_localPosition(position);
	obj->transform->Set_localRotation(rotation);
	return obj;
}

shared_ptr<Object> Object::Instantiate(std::weak_ptr<BeastEngine::Object> original, Vector3 position, Vector3 euler, shared_ptr<Transform> parent)
{
	shared_ptr<Object> obj = make_shared<Object>(*original.lock());
	obj->transform->Set_parent(parent);
	obj->transform->Set_localPosition(position);
	obj->transform->Set_localEulerAngles(euler);
	return obj;
}

shared_ptr<Object> Object::Instantiate(std::weak_ptr<BeastEngine::Object> original, Vector3 position, Vector4 rotation, shared_ptr<Transform> parent)
{
	shared_ptr<Object> obj = make_shared<Object>(*original.lock());
	obj->transform->Set_parent(parent);
	obj->transform->Set_localPosition(position);
	obj->transform->Set_localRotation(rotation);
	return obj;
}
*/