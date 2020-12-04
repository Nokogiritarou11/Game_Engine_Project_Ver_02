#include "Bullet.h"
#include "Dragon_HP.h"
#include "Object_Pool.h"
#include "Enemy_Collider.h"
using namespace std;

void Bullet::Start()
{
	dragon_hp = GameObject::Find("Dragon").lock()->GetComponent<Dragon_HP>();
	obj_pool = GameObject::Find("Object_Pool").lock()->GetComponent<Object_Pool>();
}

void Bullet::Update()
{
	transform->Set_position(transform->Get_position() + transform->Get_forward() * speed * Time::deltaTime);

	shared_ptr<Dragon_HP> dragon = dragon_hp.lock();
	shared_ptr<Enemy_Collider> col;
	for (weak_ptr<Enemy_Collider> c : dragon->colliders)
	{
		col = c.lock();
		if (Vector3::Distance(transform->Get_position(), col->transform->Get_position()) <= col->Size)
		{
			dragon->Damage(static_cast<int>(col->Damage_Magnification * 5.0f));

			if (shared_ptr<GameObject> obj = obj_pool.lock()->Instance_Hit())
			{
				obj->transform->Set_position(transform->Get_position());
			}

			gameObject->SetActive(false);
			break;
		}
	}
}

bool Bullet::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Bullet");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Bullet_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Bullet>(shared_from_this()));
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