#include "Object_Pool.h"
using namespace std;

void Object_Pool::Start()
{
	for (int i = 0; i < 50; ++i)
	{
		shared_ptr<GameObject> obj = Resources::Load_Prefab("Resouces/Prefab/Bullet.prefab");
		obj->SetActive(false);
		Bullet_List.emplace_back(obj);
	}

	for (int i = 0; i < 10; ++i)
	{
		shared_ptr<GameObject> obj = Resources::Load_Prefab("Resouces/Prefab/Bomb.prefab");
		obj->SetActive(false);
		Bomb_List.emplace_back(obj);
	}
}

void Object_Pool::Update()
{

}

weak_ptr<GameObject> Object_Pool::Instance_Bullet()
{
	weak_ptr<GameObject> obj;
	for (size_t i = 0; i < Bullet_List.size(); ++i)
	{
		if (!Bullet_List[i].lock()->activeSelf())
		{
			obj = Bullet_List[i];
			obj.lock()->SetActive(true);
			break;
		}
	}
	return obj;
}

weak_ptr<GameObject> Object_Pool::Instance_Bomb()
{
	weak_ptr<GameObject> obj;
	for (size_t i = 0; i < Bomb_List.size(); ++i)
	{
		if (!Bomb_List[i].lock()->activeSelf())
		{
			obj = Bomb_List[i];
			obj.lock()->SetActive(true);
			break;
		}
	}
	return obj;
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