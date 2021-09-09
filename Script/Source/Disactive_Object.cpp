#include "Disactive_Object.h"

using namespace std;
using namespace BeastEngine;

void Disactive_Object::Update()
{
	timer += Time::delta_time;
	if (disactive_time <= timer)
	{
		if (remove_parent)
		{
			transform->Set_Parent(nullptr);
		}
		gameobject->Set_Active(false);
		timer = 0;
	}
}

bool Disactive_Object::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Disactive_Object", open)) return false;

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_Checkbox("remove_parent", "##remove_parent", &remove_parent, window_center);
		ImGui::LeftText_DragFloat("disactive_time", "##disactive_time", &disactive_time, window_center, -FLT_MIN, 1.0f);
	}
	return true;
}