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
	bool open = false;
	if (!Draw_ImGui_Header("Sphere_Collider", open)) return false;

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::Text(u8"”¼Œa");
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