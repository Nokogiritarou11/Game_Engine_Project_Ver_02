#include "Render_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Engine.h"
using namespace std;

void Render_Manager::Reset()
{
	Renderer_3D_list.clear();
	Renderer_2D_list.clear();
	Camera_list.clear();
}

void Render_Manager::Add(shared_ptr<Mesh_Renderer> m_rend)
{
	Renderer_3D_list.emplace_back(m_rend);
}
void Render_Manager::Add(shared_ptr<SkinMesh_Renderer> m_rend)
{
	Renderer_3D_list.emplace_back(m_rend);
}
void Render_Manager::Add(shared_ptr<Sprite_Renderer> m_rend)
{
	Renderer_2D_list.emplace_back(m_rend);
}
void Render_Manager::Add(shared_ptr<Camera> mono)
{
	Camera_list.emplace_back(mono);
}

void Render_Manager::Render()
{
	for (list<weak_ptr<Camera>>::iterator itr = Camera_list.begin(); itr != Camera_list.end();)
	{
		if (itr->expired())
		{
			itr = Camera_list.erase(itr);
			continue;
		}
		shared_ptr<Camera> camera = itr->lock();
		if (camera->gameObject->activeSelf())
		{
			if (camera->enableSelf())
			{
				// プロジェクション行列を作成
				{
					// 角度をラジアン(θ)に変換
					float fov_y = XMConvertToRadians(camera->FOV);	// 画角
					float aspect = (float)Engine::view_game->screen_x / (float)Engine::view_game->screen_y;	// 画面比率

					XMStoreFloat4x4(&camera->P, XMMatrixPerspectiveFovLH(fov_y, aspect, camera->near_z, camera->far_z));

					//XMStoreFloat4x4(&P, XMMatrixOrthographicLH(100.0f, 100.0f, 0.1f, 1000.0f));
				}
				// ビュー行列を作成
				// カメラの設定
				{
					Vector3 pos = camera->transform->Get_position();
					Vector4 eye = { pos.x,pos.y,pos.z ,0 };
					XMVECTOR eye_v = XMLoadFloat4(&eye);

					XMVECTOR focus_v = eye_v + XMLoadFloat3(&camera->transform->Get_forward());

					XMVECTOR camForward = XMVector3Normalize(focus_v - eye_v);    // Get forward vector based on target
					camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
					camForward = XMVector3Normalize(camForward);
					XMVECTOR camRight = XMVectorSet(-XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward), 0.0f);

					XMVECTOR up_v = camera->transform->Get_up();
					XMStoreFloat4(&camera->focus, focus_v);
					XMStoreFloat4x4(&camera->V, XMMatrixLookAtLH(eye_v, focus_v, up_v));
				}
				Engine::view_game->Render(camera->V, camera->P, camera->transform);
			}
		}
		itr++;
	}
}