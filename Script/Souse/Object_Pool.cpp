#include "Object_Pool.h"
using namespace std;

void Object_Pool::Start()
{
	for (int i = 0; i < 20; ++i)
	{
		shared_ptr<GameObject> obj = Resources::Load_Prefab("Resouces/Prefab/Bullet.prefab");
		obj->SetActive(false);
		Bullet_List.emplace_back(obj);
	}

	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<GameObject> obj = Resources::Load_Prefab("Resouces/Prefab/Bomb.prefab");
		obj->SetActive(false);
		Bomb_List.emplace_back(obj);
	}

	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<GameObject> obj = Resources::Load_Prefab("Resouces/Prefab/Bomb_Explosion.prefab");
		obj->SetActive(false);
		Bomb_Explosion_List.emplace_back(obj);
	}

	for (int i = 0; i < 20; ++i)
	{
		shared_ptr<GameObject> obj = Resources::Load_Prefab("Resouces/Prefab/Hit.prefab");
		obj->SetActive(false);
		Hit_List.emplace_back(obj);
	}

	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<GameObject> obj = Resources::Load_Prefab("Resouces/Prefab/Breath.prefab");
		obj->SetActive(false);
		Breath_List.emplace_back(obj);
	}
	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<GameObject> obj = Resources::Load_Prefab("Resouces/Prefab/Breath_Explosion.prefab");
		obj->SetActive(false);
		Breath_Explosion_List.emplace_back(obj);
	}
	for (int i = 0; i < 20; ++i)
	{
		shared_ptr<GameObject> obj = Resources::Load_Prefab("Resouces/Prefab/Breath_Round_Collision.prefab");
		obj->SetActive(false);
		Breath_Round_List.emplace_back(obj);
	}
}

shared_ptr<GameObject> Object_Pool::Instance_Bullet()
{
	shared_ptr<GameObject> obj;
	for (size_t i = 0; i < Bullet_List.size(); ++i)
	{
		if (!Bullet_List[i].lock()->activeSelf())
		{
			obj = Bullet_List[i].lock();
			obj->SetActive(true);
			return obj;
		}
	}
	return nullptr;
}

shared_ptr<GameObject> Object_Pool::Instance_Bomb()
{
	shared_ptr<GameObject> obj;
	for (size_t i = 0; i < Bomb_List.size(); ++i)
	{
		if (!Bomb_List[i].lock()->activeSelf())
		{
			obj = Bomb_List[i].lock();
			obj->SetActive(true);
			return obj;
		}
	}
	return nullptr;
}

shared_ptr<GameObject> Object_Pool::Instance_Bomb_Explosion()
{
	shared_ptr<GameObject> obj;
	for (size_t i = 0; i < Bomb_Explosion_List.size(); ++i)
	{
		if (!Bomb_Explosion_List[i].lock()->activeSelf())
		{
			obj = Bomb_Explosion_List[i].lock();
			obj->SetActive(true);
			return obj;
		}
	}
	return nullptr;
}

shared_ptr<GameObject> Object_Pool::Instance_Hit()
{
	shared_ptr<GameObject> obj;
	for (size_t i = 0; i < Hit_List.size(); ++i)
	{
		if (!Hit_List[i].lock()->activeSelf())
		{
			obj = Hit_List[i].lock();
			obj->SetActive(true);
			return obj;
		}
	}
	return nullptr;
}

shared_ptr<GameObject> Object_Pool::Instance_Breath()
{
	shared_ptr<GameObject> obj;
	for (size_t i = 0; i < Breath_List.size(); ++i)
	{
		if (!Breath_List[i].lock()->activeSelf())
		{
			obj = Breath_List[i].lock();
			obj->SetActive(true);
			return obj;
		}
	}
	return nullptr;
}

shared_ptr<GameObject> Object_Pool::Instance_Breath_Explosion()
{
	shared_ptr<GameObject> obj;
	for (size_t i = 0; i < Breath_Explosion_List.size(); ++i)
	{
		if (!Breath_Explosion_List[i].lock()->activeSelf())
		{
			obj = Breath_Explosion_List[i].lock();
			obj->SetActive(true);
			return obj;
		}
	}
	return nullptr;
}

shared_ptr<GameObject> Object_Pool::Instance_Breath_Round()
{
	shared_ptr<GameObject> obj;
	for (size_t i = 0; i < Breath_Round_List.size(); ++i)
	{
		if (!Breath_Round_List[i].lock()->activeSelf())
		{
			obj = Breath_Round_List[i].lock();
			obj->SetActive(true);
			return obj;
		}
	}
	return nullptr;
}

bool Object_Pool::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Object_Pool");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Object_Pool_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Object_Pool>(shared_from_this()));
			removed = false;
		}
		ImGui::EndPopup();
	}
	if (!removed)
	{
		return false;
	}

	if (open)
	{
	}
	return true;
}