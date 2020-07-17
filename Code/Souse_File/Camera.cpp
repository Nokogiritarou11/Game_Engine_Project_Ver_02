#include "Camera.h"
#include "DxSystem.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera_Manager.h"
#include "Include_ImGui.h"
using namespace DirectX;

using namespace std;

void Camera::Initialize()
{
	Camera_Manager::Add(static_pointer_cast<Camera>(shared_from_this()));
	DxSystem::DeviceContext->RSGetViewports(&num_viewports, &viewport);
}

void Camera::Initialize(std::shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;
	Camera_Manager::Add(static_pointer_cast<Camera>(shared_from_this()));
	DxSystem::DeviceContext->RSGetViewports(&num_viewports, &viewport);
}

void Camera::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::DragFloat("FOV", &FOV, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat(u8"�ŒZ�`�拗��", &near_z, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat(u8"�Œ��`�拗��", &far_z, 0.1f, -FLT_MAX, FLT_MAX);
	}
}

void Camera::Update()
{
	// �v���W�F�N�V�����s����쐬
	// ��ʃT�C�Y�擾�̂��߃r���[�|�[�g���擾
	{
		// �p�x�����W�A��(��)�ɕϊ�
		float fov_y = XMConvertToRadians(FOV);	// ��p
		float aspect = viewport.Width / viewport.Height;	// ��ʔ䗦

		XMStoreFloat4x4(&P, XMMatrixPerspectiveFovLH(fov_y, aspect, near_z, far_z));
	}
	// �r���[�s����쐬
	// �J�����̐ݒ�
	{
		Vector3 pos = transform->Get_position();
		Vector4 eye = { pos.x,pos.y,pos.z ,0 };
		XMVECTOR eye_v = XMLoadFloat4(&eye);

		XMVECTOR focus_v = eye_v + XMLoadFloat3(&transform->Get_forward());

		XMVECTOR camForward = XMVector3Normalize(focus_v - eye_v);    // Get forward vector based on target
		camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
		camForward = XMVector3Normalize(camForward);
		XMVECTOR camRight = XMVectorSet(-XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward), 0.0f);

		XMVECTOR up_v = XMVectorSet(0, 1, 0, 0);
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