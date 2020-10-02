#include "Camera.h"
#include "DxSystem.h"
#include "GameObject.h"
#include "Include_ImGui.h"
#include "Render_Manager.h"
using namespace DirectX;

using namespace std;

void Camera::Initialize()
{
	Render_Manager::Add(static_pointer_cast<Camera>(shared_from_this()));
	DxSystem::DeviceContext->RSGetViewports(&num_viewports, &viewport);
}

void Camera::Initialize(std::shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;
	Render_Manager::Add(static_pointer_cast<Camera>(shared_from_this()));
	DxSystem::DeviceContext->RSGetViewports(&num_viewports, &viewport);
}

bool Camera::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Camera"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Camera_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<Camera>(shared_from_this()));
				removed = false;
			}
			ImGui::EndPopup();
			if (!removed)
			{
				return false;
			}
		}

		ImGui::DragFloat("FOV", &FOV, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat(u8"最短描画距離", &near_z, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat(u8"最長描画距離", &far_z, 0.1f, -FLT_MAX, FLT_MAX);
	}
	return true;
}

void Camera::Update(u_int width, u_int height)
{
	// プロジェクション行列を作成
	// 画面サイズ取得のためビューポートを取得
	{
		// 角度をラジアン(θ)に変換
		float fov_y = XMConvertToRadians(FOV);	// 画角
		float aspect = (float)width / (float)height;	// 画面比率

		XMStoreFloat4x4(&P, XMMatrixPerspectiveFovLH(fov_y, aspect, near_z, far_z));
	}
	// ビュー行列を作成
	// カメラの設定
	{
		Vector3 pos = transform->Get_position();
		Vector4 eye = { pos.x,pos.y,pos.z ,0 };
		XMVECTOR eye_v = XMLoadFloat4(&eye);

		XMVECTOR focus_v = eye_v + XMLoadFloat3(&transform->Get_forward());

		XMVECTOR camForward = XMVector3Normalize(focus_v - eye_v);    // Get forward vector based on target
		camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
		camForward = XMVector3Normalize(camForward);
		XMVECTOR camRight = XMVectorSet(-XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward), 0.0f);

		XMVECTOR up_v = transform->Get_up();
		XMStoreFloat4(&focus, focus_v);
		XMStoreFloat4x4(&V, XMMatrixLookAtLH(eye_v, focus_v, up_v));
	}
}

Vector2 Camera::WorldToViewportPoint(Vector3 pos)
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
			XMLoadFloat4x4(&P),
			XMLoadFloat4x4(&V),
			XMMatrixIdentity()
		);
	Vector2 re;
	XMStoreFloat2(&re, screen);
	return re;
}