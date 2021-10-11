#include "Disable_Object.h"

using namespace std;
using namespace BeastEngine;

void Disable_Object::Update()
{
	timer += Time::delta_time;
	if (disable_time <= timer)
	{
		if (remove_parent)
		{
			transform->Set_Parent(nullptr);
		}
		gameobject->Set_Active(false);
		timer = 0;
	}
}

bool Disable_Object::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Disable_Object", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_Checkbox("remove_parent", "##remove_parent", &remove_parent, window_center);
		ImGui::LeftText_DragFloat("disable_time", "##disable_time", &disable_time, window_center, -FLT_MIN, 1.0f);
	}
	return true;
}