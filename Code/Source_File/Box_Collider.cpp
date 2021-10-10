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
	bool open = false;
	if (!Draw_ImGui_Header("Box_Collider", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::Text(u8"ÉTÉCÉY");
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