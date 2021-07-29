#include "Sphere_Collider.h"
#include "Include_ImGui.h"

using namespace std;
using namespace BeastEngine;

void Sphere_Collider::Create_Shape()
{
	shape = make_unique<btSphereShape>(radius);
}

void Sphere_Collider::Set_Radius(float new_radius)
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

bool Sphere_Collider::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Sphere_Collider", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	if (ImGui::BeginPopupContextItem("Sphere_Collider_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Sphere_Collider>(shared_from_this()));
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
		ImGui::SetNextItemWidth(window_center);
		float rad = radius;
		if (ImGui::DragFloat("##Sphere_Radius", &rad, 0.1f))
		{
			Set_Radius(rad);
		}
		Draw_ImGui_Common();
	}
	return true;
}