#include "Box_Collider.h"
#include "Include_ImGui.h"

using namespace std;
using namespace BeastEngine;

void Box_Collider::Create_Shape()
{
	shape = make_unique<btBoxShape>(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
}

void Box_Collider::Set_Size(Vector3& new_size)
{
	if (size != new_size)
	{
		size = new_size;
		Reset_Collider();
		Create_Shape();
		Create_Collider();
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
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::Text(u8"サイズ");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(window_center);
		float size_im[3] = { size.x,size.y,size.z };
		if (ImGui::DragFloat3("##Box_Size", size_im, 0.1f))
		{
			Vector3 new_size = { size_im[0], size_im[1], size_im[2] };
			Set_Size(new_size);
		}
		Draw_ImGui_Common();
	}
	return true;
}