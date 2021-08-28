#include "Capsule_Collider.h"
#include "Include_ImGui.h"

using namespace std;
using namespace BeastEngine;

void Capsule_Collider::Create_Shape()
{
	shape = make_unique<btCapsuleShape>(radius, height);
}

void Capsule_Collider::Set_Radius(float new_radius)
{
	if (radius != new_radius)
	{
		radius = new_radius;
		Reset_Collider();
		Create_Shape();
		Create_Collider();
		Rescale_Shape();
	}
}

void Capsule_Collider::Set_Height(float new_height)
{
	if (height != new_height)
	{
		height = new_height;
		Reset_Collider();
		Create_Shape();
		Create_Collider();
		Rescale_Shape();
	}
}

bool Capsule_Collider::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Capsule_Collider", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	if (ImGui::BeginPopupContextItem("Capsule_Collider_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Capsule_Collider>(shared_from_this()));
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

		ImGui::Text(u8"半径");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		float rad = radius;
		if (ImGui::DragFloat("##Capsule_Radius", &rad, 0.1f))
		{
			Set_Radius(rad);
		}

		ImGui::Text(u8"高さ");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		float hei = height;
		if (ImGui::DragFloat("##Capsule_Height", &hei, 0.1f))
		{
			Set_Height(hei);
		}

		Draw_ImGui_Common();
	}
	return true;
}