#include "Box_Collider.h"
#include "Engine.h"
#include "Collider_Manager.h"
#include "Asset_Manager.h"
#include "Transform.h"
#include "GameObject.h"
#include "Include_ImGui.h"

using namespace std;
using namespace BeastEngine;

Vector3 Box_Collider::Get_Scaling_Size() const
{
	Vector3 v = size;
	v *= transform->Get_Scale();
	return v;
}

void Box_Collider::Initialize(shared_ptr<GameObject> obj)
{
	gameobject = obj;
	Engine::asset_manager->Registration_Asset(shared_from_this());
	transform = obj->transform;
	Set_Active(gameobject->Get_Active_In_Hierarchy());
}


void Box_Collider::Set_Active(bool value)
{
	if (value)
	{
		if (gameobject->Get_Active_In_Hierarchy())
		{
			if (Get_Enabled())
			{
				if (!is_called)
				{
					Initialize_MonoBehaviour();
					Engine::collider_manager->Add(static_pointer_cast<Box_Collider>(shared_from_this()));
					is_called = true;
				}
			}
		}
	}
}

bool Box_Collider::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Box_Collider", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	if (ImGui::BeginPopupContextItem("Box_Collider_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Box_Collider>(shared_from_this()));
			removed = true;
		}
		ImGui::EndPopup();
	}
	if (removed)
	{
		return false;
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
	bool enable = Get_Enabled();
	if (ImGui::Checkbox("##enable", &enable))
	{
		Set_Enabled(enable);
	}

	if (open)
	{

	}
	return true;
}