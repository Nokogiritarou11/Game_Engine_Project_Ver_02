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

void Camera::Initialize(const shared_ptr<GameObject>& obj)
{
	//アセットマネージャーへの登録とComponentの初期化
	Engine::asset_manager->Registration_Asset(shared_from_this());
	gameobject = obj;
	transform = obj->transform;

	Engine::render_manager->Add(static_pointer_cast<Camera>(shared_from_this()));
	DxSystem::device_context->RSGetViewports(&num_viewports, &viewport);
}

void Camera::Update(const float screen_x, const float screen_y)
{
	// ReverseZ行列
	static constexpr Matrix reverse = { 1,0,0,0,
										0,1,0,0,
										0,0,-1,0,
										0,0,1,1 };

	// プロジェクション行列を作成
	if (is_orthographic)
	{
		projection_matrix = XMMatrixOrthographicRH(orthographic_size, orthographic_size, near_z, far_z);
		projection_matrix = projection_matrix * reverse;
	}
	else
	{
		const float fov_y = XMConvertToRadians(fov);// 画角
		const float aspect = screen_x / screen_y;	// 画面比率
		projection_matrix = XMMatrixPerspectiveFovRH(fov_y, aspect, near_z, far_z);
		projection_matrix = projection_matrix * reverse;
		Engine::particle_manager->Camera_Update(transform, fov_y, near_z, far_z, aspect);
	}

	// ビュー行列を作成
	const Vector3 eye_v = transform->Get_Position();
	const Vector3 focus_v = eye_v + transform->Get_Forward();

	Vector3 cam_forward = XMVector3Normalize(focus_v - eye_v);
	cam_forward = XMVectorSetY(cam_forward, 0.0f);
	cam_forward = XMVector3Normalize(cam_forward);

	const XMVECTOR up_v = transform->Get_Up();
	view_matrix = XMMatrixLookAtRH(eye_v, focus_v, up_v);

	view_projection_matrix = view_matrix * projection_matrix;

	//視錐台化リング用のPlaneを作成
	//左
	frustum_planes[0].x = view_projection_matrix._14 + view_projection_matrix._11;
	frustum_planes[0].y = view_projection_matrix._24 + view_projection_matrix._21;
	frustum_planes[0].z = view_projection_matrix._34 + view_projection_matrix._31;
	frustum_planes[0].w = view_projection_matrix._44 + view_projection_matrix._41;

	//右
	frustum_planes[1].x = view_projection_matrix._14 - view_projection_matrix._11;
	frustum_planes[1].y = view_projection_matrix._24 - view_projection_matrix._21;
	frustum_planes[1].z = view_projection_matrix._34 - view_projection_matrix._31;
	frustum_planes[1].w = view_projection_matrix._44 - view_projection_matrix._41;

	//上
	frustum_planes[2].x = view_projection_matrix._14 - view_projection_matrix._12;
	frustum_planes[2].y = view_projection_matrix._24 - view_projection_matrix._22;
	frustum_planes[2].z = view_projection_matrix._34 - view_projection_matrix._32;
	frustum_planes[2].w = view_projection_matrix._44 - view_projection_matrix._42;

	//下
	frustum_planes[3].x = view_projection_matrix._14 + view_projection_matrix._12;
	frustum_planes[3].y = view_projection_matrix._24 + view_projection_matrix._22;
	frustum_planes[3].z = view_projection_matrix._34 + view_projection_matrix._32;
	frustum_planes[3].w = view_projection_matrix._44 + view_projection_matrix._42;

	//Near
	frustum_planes[4].x = view_projection_matrix._13;
	frustum_planes[4].y = view_projection_matrix._23;
	frustum_planes[4].z = view_projection_matrix._33;
	frustum_planes[4].w = view_projection_matrix._43;

	//Far
	frustum_planes[5].x = view_projection_matrix._14 - view_projection_matrix._13;
	frustum_planes[5].y = view_projection_matrix._24 - view_projection_matrix._23;
	frustum_planes[5].z = view_projection_matrix._34 - view_projection_matrix._33;
	frustum_planes[5].w = view_projection_matrix._44 - view_projection_matrix._43;

	//正規化
	for (size_t i = 0; i < 6; ++i)
	{
		const float length = sqrt((frustum_planes[i].x * frustum_planes[i].x) + (frustum_planes[i].y * frustum_planes[i].y) + (frustum_planes[i].z * frustum_planes[i].z));
		frustum_planes[i].x /= length;
		frustum_planes[i].y /= length;
		frustum_planes[i].z /= length;
		frustum_planes[i].w /= length;
	}
}

bool Camera::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = false;
	if (!Draw_ImGui_Header("Camera", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

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

Vector2 Camera::World_To_Viewport_Point(const Vector3 pos) const
{
	const XMVECTOR p = XMLoadFloat3(&pos);
	const XMVECTOR screen
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
	re.x /= viewport.Width;
	re.y /= viewport.Height;
	return re;
}

Vector2 Camera::World_To_Screen_Point(const Vector3 pos) const
{
	const XMVECTOR p = XMLoadFloat3(&pos);
	const XMVECTOR screen
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
