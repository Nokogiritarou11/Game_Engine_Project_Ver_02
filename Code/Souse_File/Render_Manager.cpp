#include "Render_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Engine.h"
#include "View_Game.h"
#include "Particle_Manager.h"
#include "Renderer.h"
#include "Camera.h"
#include "Original_Math.h"
using namespace DirectX;
using namespace std;
using namespace BeastEngine;

void Render_Manager::Reset()
{
	for (weak_ptr<Renderer> r : Renderer_3D_list)
	{
		if (shared_ptr<Renderer> rend = r.lock())
		{
			rend->Disable_flg = false;
			rend->enabled_old = false;
			rend->IsCalled = false;
		}
	}
	for (weak_ptr<Renderer> r : Renderer_2D_list)
	{
		if (shared_ptr<Renderer> rend = r.lock())
		{
			rend->Disable_flg = false;
			rend->enabled_old = false;
			rend->IsCalled = false;
		}
	}
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

void Render_Manager::Check_Renderer()
{
	shared_ptr<Renderer> p_rend = nullptr;
	//3Dオブジェクト
	{
		bool expired = false;
		bool disabled = false;
		for (weak_ptr<Renderer> r : Renderer_3D_list)
		{
			if (!r.expired())
			{
				p_rend = r.lock();
				p_rend->Recalculated_Constant_Buffer = false;
				if (!p_rend->gameObject->activeInHierarchy())
				{
					p_rend->Disable_flg = true;
					disabled = true;
				}
			}
			else
			{
				expired = true;
			}
		}
		if (expired)
		{
			auto removeIt = remove_if(Renderer_3D_list.begin(), Renderer_3D_list.end(), [](weak_ptr<Renderer> r) { return r.expired(); });
			Renderer_3D_list.erase(removeIt, Renderer_3D_list.end());
		}
		if (disabled)
		{
			auto removeIt = remove_if(Renderer_3D_list.begin(), Renderer_3D_list.end(), [](weak_ptr<Renderer> r) { shared_ptr<Renderer> rend = r.lock(); rend->IsCalled = false; return rend->Disable_flg; });
			Renderer_3D_list.erase(removeIt, Renderer_3D_list.end());
		}
	}
	//2Dオブジェクト
	{
		bool expired = false;
		bool disabled = false;
		for (weak_ptr<Renderer> r : Renderer_2D_list)
		{
			if (!r.expired())
			{
				p_rend = r.lock();
				p_rend->Recalculated_Constant_Buffer = false;
				if (!p_rend->gameObject->activeInHierarchy())
				{
					p_rend->Disable_flg = true;
					disabled = true;
				}
			}
			else
			{
				expired = true;
			}
		}
		if (expired)
		{
			auto removeIt = remove_if(Renderer_2D_list.begin(), Renderer_2D_list.end(), [](weak_ptr<Renderer> r) { return r.expired(); });
			Renderer_2D_list.erase(removeIt, Renderer_2D_list.end());
		}
		if (disabled)
		{
			auto removeIt = remove_if(Renderer_2D_list.begin(), Renderer_2D_list.end(), [](weak_ptr<Renderer> r) { shared_ptr<Renderer> rend = r.lock(); rend->IsCalled = false; return rend->Disable_flg; });
			Renderer_2D_list.erase(removeIt, Renderer_2D_list.end());
		}
	}
}

void Render_Manager::Render()
{
	Check_Renderer();

	shared_ptr<Camera> camera = nullptr;
	bool expired = false;
	for (weak_ptr<Camera> c : Camera_list)
	{
		if (!c.expired())
		{
			camera = c.lock();
			if (camera->gameObject->activeInHierarchy())
			{
				if (camera->enableSelf())
				{
					// プロジェクション行列を作成
					float fov_y = XMConvertToRadians(camera->FOV);	// 画角
					float aspect = (float)Engine::view_game->screen_x / (float)Engine::view_game->screen_y;	// 画面比率

					camera->P = XMMatrixPerspectiveFovRH(fov_y, aspect, camera->near_z, camera->far_z);

					// ビュー行列を作成
					//{
					Vector3 eye_v = camera->transform->Get_position();
					Vector3 focus_v = eye_v + camera->transform->Get_forward();

					Vector3 camForward = XMVector3Normalize(focus_v - eye_v);    // Get forward vector based on target
					camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
					camForward = XMVector3Normalize(camForward);
					Vector3 camRight = { -XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward) };

					XMVECTOR up_v = camera->transform->Get_up();
					camera->V = XMMatrixLookAtRH(eye_v, focus_v, up_v);
					//}

					Engine::particle_manager->Camera_Update(camera->transform, fov_y, camera->near_z, camera->far_z, aspect);
					Engine::view_game->Render(camera->V, camera->P, camera->transform);
				}
			}
		}
		else
		{
			expired = true;
		}
	}
	if (expired)
	{
		auto removeIt = remove_if(Camera_list.begin(), Camera_list.end(), [](weak_ptr<Camera> m) { return m.expired(); });
		Camera_list.erase(removeIt, Camera_list.end());
	}
}