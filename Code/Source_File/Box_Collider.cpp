#include "Box_Collider.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include "Transform.h"
#include "GameObject.h"
#include "Include_ImGui.h"
#include "RigidBody.h"

using namespace std;
using namespace BeastEngine;

void Box_Collider::Create_Shape()
{
	shape = make_unique<btBoxShape>(btVector3(size.x, size.y, size.z));
}

void Box_Collider::Set_Size(Vector3& new_size)
{
	if (size != new_size)
	{
		size = new_size;
		if (is_trigger)
		{
			ghost->ghost.reset();
		}
		else
		{
			rigidbody->rigidbody.reset();
		}
		Create_Shape();
		Rescale_Shape();
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
		ImGui::Text(u8"サイズ");
		float size_im[3] = { size.x,size.y,size.z };
		if (ImGui::InputFloat3("##Box_Size", size_im, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
		{
			Vector3 new_size = { size_im[0], size_im[1], size_im[2] };
			Set_Size(new_size);
		}
		ImGui::Separator();
		Draw_ImGui_Common();
	}
	return true;
}