#include "Bomb.h"
#include "Dragon_HP.h"
#include "Enemy_Collider.h"
using namespace std;

void Bomb::Start()
{
	dragon_hp = GameObject::Find("Dragon").lock()->GetComponent<Dragon_HP>();
}

void Bomb::Update()
{
	transform->Set_position(transform->Get_position() + transform->Get_forward() * speed * Time::deltaTime);
	Vector3 e = transform->Get_eulerAngles();
	transform->Set_eulerAngles(e.x + 30.0f * Time::deltaTime, e.y, e.z);

	if (transform->Get_position().y <= 0)
	{
		/*
		if (shared_ptr<GameObject> obj = obj_pool.lock()->Instance_Breath_Explosion())
		{
			obj->transform->Set_position(transform->Get_position());
			gameObject->SetActive(false);
		}*/
	}

	shared_ptr<Dragon_HP> dragon = dragon_hp.lock();
	shared_ptr<Enemy_Collider> col;
	for (weak_ptr<Enemy_Collider> c : dragon->colliders)
	{
		col = c.lock();
		if (Vector3::Distance(transform->Get_position(), col->transform->Get_position()) <= col->Size)
		{
			dragon->Damage(static_cast<int>(col->Damage_Magnification * 25.0f));
			gameObject->SetActive(false);
			break;
		}
	}
}

bool Bomb::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Bomb");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Bomb_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Bomb>(shared_from_this()));
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
		ImGui::DragFloat("speed", &speed, 0.1f, 0.0f, FLT_MAX);
	}
	return true;
}