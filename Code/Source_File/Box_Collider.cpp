#include "Box_Collider.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include "Transform.h"
#include "GameObject.h"
#include "Include_ImGui.h"
#include "RigidBody.h"
#include "BulletPhysics_Manager.h"

using namespace std;
using namespace BeastEngine;

void Box_Collider::Create_Shape()
{
	shape = make_unique<btBoxShape>(btVector3(size.x, size.y, size.z));
	Resize_Shape();
}

void Box_Collider::Resize_Shape()
{
	Vector3 scale = transform->Get_Scale();
	btVector3 bt_scale(scale.x, scale.y, scale.z);
	if (shape->getLocalScaling() != bt_scale)
	{
		shape->setLocalScaling(bt_scale);
		if (is_trigger)
		{
			Engine::bulletphysics_manager->Resize_Ghost(ghost);
		}
		else
		{
			Engine::bulletphysics_manager->Resize_RigidBody(rigidbody->rigidbody);
		}
	}
}

void Box_Collider::Set_Size(Vector3& new_size)
{
	if (size != new_size)
	{
		size = new_size;
		if (is_trigger)
		{
			Engine::bulletphysics_manager->Remove_Ghost(ghost);
		}
		else
		{
			Engine::bulletphysics_manager->Remove_RigidBody(rigidbody->rigidbody);
			rigidbody->rigidbody.reset();
		}
		Create_Shape();
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