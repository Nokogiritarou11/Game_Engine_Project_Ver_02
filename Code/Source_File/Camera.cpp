#include "Camera.h"
#include "DxSystem.h"
#include "GameObject.h"
#include "Include_ImGui.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include "Render_Manager.h"
using namespace DirectX;
using namespace BeastEngine;
using namespace std;

void Camera::Initialize(std::shared_ptr<GameObject> obj)
{
	gameobject = obj;
	Engine::asset_manager->Registration_Asset(shared_from_this());
	transform = obj->transform;
	Engine::render_manager->Add(static_pointer_cast<Camera>(shared_from_this()));
	DxSystem::device_context->RSGetViewports(&num_viewports, &viewport);
}

bool Camera::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	if (ImGui::BeginPopupContextItem("Camera_sub"))
	{
		if (ImGui::Selectable(u8"�R���|�[�l���g���폜"))
		{
			Object::Destroy(dynamic_pointer_cast<Camera>(shared_from_this()));
			removed = true;
		}
		ImGui::EndPopup();
		if (removed)
		{
			return false;
		}
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

		ImGui::Text("FOV");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(window_center);
		ImGui::DragFloat("##FOV", &fov, 0.1f, 0.01f, FLT_MAX);

		ImGui::Text(u8"�ŒZ�`�拗��");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(window_center);
		ImGui::DragFloat("##Near", &near_z, 0.1f, -FLT_MAX, FLT_MAX);

		ImGui::Text(u8"�Œ��`�拗��");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(window_center);
		ImGui::DragFloat("##Far", &far_z, 0.1f, -FLT_MAX, FLT_MAX);
	}
	return true;
}

Vector2 Camera::World_To_ViewportPoint(Vector3 pos)
{
	XMVECTOR p = XMLoadFloat3(&pos);
	XMVECTOR screen
		= XMVector3Project(
			p,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			XMLoadFloat4x4(&projection_matrix),
			XMLoadFloat4x4(&view_matrix),
			XMMatrixIdentity()
		);
	Vector2 re;
	XMStoreFloat2(&re, screen);
	return re;
}