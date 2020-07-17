#include "GameObject.h"
#include "Bullet.h"
#include "SkinMesh_Renderer.h"
#include "ObjectPool.h"
using namespace std;

void ObjectPool::Start()
{
	CreatePool();
}

void ObjectPool::CreatePool()
{
	list<weak_ptr<GameObject>> g_list = {};
	for (int i = 0; i < maxCount[0]; i++)
	{
		shared_ptr<GameObject> g = GameObject::Instantiate("bullet");

		g->AddComponent<Bullet>();
		shared_ptr<SkinMesh_Renderer> renderer = g->AddComponent<SkinMesh_Renderer>();
		renderer->Set_Mesh(Mesh::Load_Mesh("Model\\", "Sphere"));

		g->SetActive(false);
		g_list.push_back(g);
	}

	_poolObj.push_back(g_list);
}

weak_ptr<GameObject> ObjectPool::Get_Object(int Number)
{
	list<weak_ptr<GameObject>>::iterator itr_end = _poolObj[Number].end();
	for (list<weak_ptr<GameObject>>::iterator itr = _poolObj[Number].begin(); itr != itr_end; itr++)
	{
		shared_ptr<GameObject> go = itr->lock();
		if (!go->activeSelf())
		{
			return go;
		}
	}

	return weak_ptr<GameObject>();
}

weak_ptr<GameObject> ObjectPool::Instance_inPool(int Number, Vector3 pos, Quaternion rot)
{
	shared_ptr<GameObject> go = Get_Object(Number).lock();
	go->transform->Set_position(pos);
	go->transform->Set_rotation(rot);
	go->SetActive(true);
	return go;
}