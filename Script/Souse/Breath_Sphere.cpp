#include "Breath_Sphere.h"
#include "Object_Pool.h"
#include "Actions.h"
using namespace std;

void Breath_Sphere::Start()
{
	obj_pool = GameObject::Find("Object_Pool").lock()->GetComponent<Object_Pool>();
}

void Breath_Sphere::Update()
{
	transform->Set_position(transform->Get_position() + transform->Get_forward() * 175 *Action::speed * Time::deltaTime);

	if (transform->Get_position().y <= 0)
	{
		if (shared_ptr<GameObject> obj = obj_pool.lock()->Instance_Breath_Explosion())
		{
			obj->transform->Set_position(transform->Get_position());
			gameObject->SetActive(false);
		}
	}
}

bool Breath_Sphere::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Breath_Sphere");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Breath_Sphere_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Breath_Sphere>(shared_from_this()));
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