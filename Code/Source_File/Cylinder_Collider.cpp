#include "Cylinder_Collider.h"
#include "Include_ImGui.h"

using namespace std;
using namespace BeastEngine;

void Cylinder_Collider::Create_Shape()
{
	shape = make_unique<btCylinderShape>(btVector3(radius, height, radius));
}

void Cylinder_Collider::Set_Radius(const float new_radius)
{
	//重いので変更時のみ
	if (radius != new_radius)
	{
		radius = new_radius;
		Reset_Collider();
		Create_Shape();
		Create_Collider();
		Rescale_Shape();
	}
}

void Cylinder_Collider::Set_Height(const float new_height)
{
	//重いので変更時のみ
	if (height != new_height)
	{
		height = new_height;
		Reset_Collider();
		Create_Shape();
		Create_Collider();
		Rescale_Shape();
	}
}

bool Cylinder_Collider::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Cylinder_Collider", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::Text(u8"半径");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		float rad = radius;
		if (ImGui::DragFloat("##Cylinder_Radius", &rad, 0.1f))
		{
			Set_Radius(rad);
		}

		ImGui::Text(u8"高さ");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		float hei = height;
		if (ImGui::DragFloat("##Cylinder_Height", &hei, 0.1f))
		{
			Set_Height(hei);
		}

		Draw_ImGui_Common();
	}
	return true;
}