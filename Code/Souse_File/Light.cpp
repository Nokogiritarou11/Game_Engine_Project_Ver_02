#include "Light.h"
#include "Engine.h"
#include "Light_Manager.h"
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

	gameobject = obj;
	transform = obj->transform;
}

bool Light::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Light_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Light>(shared_from_this()));
			removed = false;
		}
		ImGui::EndPopup();
	}
	if (!removed)
	{
		return false;
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
	static bool enable;
	enable = Get_Enabled();
	if (ImGui::Checkbox("##enable", &enable))
	{
		Set_Enabled(enable);
	}

	if (open)
	{
		float out_color[4] = { color.x,color.y,color.z,color.w };
		ImGui::ColorEdit3("Color", out_color);
		color = { out_color[0],out_color[1] ,out_color[2] ,out_color[3] };
		ImGui::DragFloat(u8"強度", &intensity, 0.01f, 0, FLT_MAX);
	}
	return true;
}