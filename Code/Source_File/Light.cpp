#include "Light.h"
#include "Engine.h"
#include "Light_Manager.h"
#include "Asset_Manager.h"
#include "GameObject.h"
#include "Object.h"
#include "Transform.h"
#include "Include_ImGui.h"
#include "Editor.h"
using namespace std;
using namespace BeastEngine;

void Light::Initialize(std::shared_ptr<GameObject> obj)
{
	Engine::light_manager->Add(static_pointer_cast<Light>(shared_from_this()));

	Engine::asset_manager->Registration_Asset(shared_from_this());
	gameobject = obj;
	transform = obj->transform;
}

bool Light::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Light", open)) return false;

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::Text("Color");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(window_center);
		float out_color[3] = { color.x,color.y,color.z };
		if (ImGui::ColorEdit3("##Color", out_color))
		{
			color = { out_color[0], out_color[1], out_color[2] };
		}

		ImGui::Text(u8"‹­“x");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(window_center);
		ImGui::DragFloat("##intensity", &intensity, 0.01f, 0, FLT_MAX);
	}
	return true;
}