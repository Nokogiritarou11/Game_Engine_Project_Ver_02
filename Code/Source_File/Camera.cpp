#include "Camera.h"
#include "DxSystem.h"
#include "GameObject.h"
#include "Include_ImGui.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include "Render_Manager.h"
#include "Particle_Manager.h"
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

void Camera::Update(float screen_x, float screen_y)
{
	// プロジェクション行列を作成
	static const Matrix reverse = { 1,0,0,0,
									0,1,0,0,
									0,0,-1,0,
									0,0,0,1 };

	if (is_orthographic)
	{
		projection_matrix = XMMatrixOrthographicRH(orthographic_size, orthographic_size, near_z, far_z);
	}
	else
	{
		float fov_y = XMConvertToRadians(fov);	// 画角
		float aspect = screen_x / screen_y;	// 画面比率
		projection_matrix = XMMatrixPerspectiveFovRH(fov_y, aspect, near_z, far_z);
		Engine::particle_manager->Camera_Update(transform, fov_y, near_z, far_z, aspect);
	}

	// ビュー行列を作成
	Vector3 eye_v = transform->Get_Position();
	Vector3 focus_v = eye_v + transform->Get_Forward();

	Vector3 camForward = XMVector3Normalize(focus_v - eye_v);
	camForward = XMVectorSetY(camForward, 0.0f);
	camForward = XMVector3Normalize(camForward);
	Vector3 camRight = { -XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward) };

	XMVECTOR up_v = transform->Get_Up();
	view_matrix = XMMatrixLookAtRH(eye_v, focus_v, up_v);

	view_projection_matrix = view_matrix * projection_matrix;

	// Left Frustum Plane
	// Add first column of the matrix to the fourth column
	frustum_planes[0].x = view_projection_matrix._14 + view_projection_matrix._11;
	frustum_planes[0].y = view_projection_matrix._24 + view_projection_matrix._21;
	frustum_planes[0].z = view_projection_matrix._34 + view_projection_matrix._31;
	frustum_planes[0].w = view_projection_matrix._44 + view_projection_matrix._41;

	// Right Frustum Plane
	// Subtract first column of matrix from the fourth column
	frustum_planes[1].x = view_projection_matrix._14 - view_projection_matrix._11;
	frustum_planes[1].y = view_projection_matrix._24 - view_projection_matrix._21;
	frustum_planes[1].z = view_projection_matrix._34 - view_projection_matrix._31;
	frustum_planes[1].w = view_projection_matrix._44 - view_projection_matrix._41;

	// Top Frustum Plane
	// Subtract second column of matrix from the fourth column
	frustum_planes[2].x = view_projection_matrix._14 - view_projection_matrix._12;
	frustum_planes[2].y = view_projection_matrix._24 - view_projection_matrix._22;
	frustum_planes[2].z = view_projection_matrix._34 - view_projection_matrix._32;
	frustum_planes[2].w = view_projection_matrix._44 - view_projection_matrix._42;

	// Bottom Frustum Plane
	// Add second column of the matrix to the fourth column
	frustum_planes[3].x = view_projection_matrix._14 + view_projection_matrix._12;
	frustum_planes[3].y = view_projection_matrix._24 + view_projection_matrix._22;
	frustum_planes[3].z = view_projection_matrix._34 + view_projection_matrix._32;
	frustum_planes[3].w = view_projection_matrix._44 + view_projection_matrix._42;

	// Near Frustum Plane
	// We could add the third column to the fourth column to get the near plane,
	// but we don't have to do this because the third column IS the near plane
	frustum_planes[4].x = view_projection_matrix._13;
	frustum_planes[4].y = view_projection_matrix._23;
	frustum_planes[4].z = view_projection_matrix._33;
	frustum_planes[4].w = view_projection_matrix._43;

	// Far Frustum Plane
	// Subtract third column of matrix from the fourth column
	frustum_planes[5].x = view_projection_matrix._14 - view_projection_matrix._13;
	frustum_planes[5].y = view_projection_matrix._24 - view_projection_matrix._23;
	frustum_planes[5].z = view_projection_matrix._34 - view_projection_matrix._33;
	frustum_planes[5].w = view_projection_matrix._44 - view_projection_matrix._43;

	for (size_t i = 0; i < 6; ++i)
	{
		float length = sqrt((frustum_planes[i].x * frustum_planes[i].x) + (frustum_planes[i].y * frustum_planes[i].y) + (frustum_planes[i].z * frustum_planes[i].z));
		frustum_planes[i].x /= length;
		frustum_planes[i].y /= length;
		frustum_planes[i].z /= length;
		frustum_planes[i].w /= length;
	}
}

bool Camera::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	if (ImGui::BeginPopupContextItem("Camera_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
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

		ImGui::Text(u8"最短描画距離");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(window_center);
		ImGui::DragFloat("##Near", &near_z, 0.1f, -FLT_MAX, FLT_MAX);

		ImGui::Text(u8"最長描画距離");
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