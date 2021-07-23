#include "Render_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Engine.h"
#include "View_Game.h"
#include "Particle_Manager.h"
#include "Renderer.h"
#include "Camera.h"
#include "Original_Math.h"
#include "SkinMesh_Renderer.h"
#include "Mesh_Renderer.h"
#include "Sprite_Renderer.h"
using namespace DirectX;
using namespace std;
using namespace BeastEngine;

void Render_Manager::Reset()
{
	for (auto& r : renderer_3D_list)
	{
		if (auto& rend = r.lock())
		{
			rend->enabled_old = false;
			rend->is_called = false;
		}
	}
	for (auto& r : renderer_2D_list)
	{
		if (auto& rend = r.lock())
		{
			rend->enabled_old = false;
			rend->is_called = false;
		}
	}
	renderer_3D_list.clear();
	renderer_2D_list.clear();
	camera_list.clear();
}

void Render_Manager::Add(shared_ptr<Mesh_Renderer> m_rend)
{
	renderer_3D_list.emplace_back(m_rend);
}
void Render_Manager::Add(shared_ptr<SkinMesh_Renderer> m_rend)
{
	renderer_3D_list.emplace_back(m_rend);
}
void Render_Manager::Add(shared_ptr<Sprite_Renderer> m_rend)
{
	renderer_2D_list.emplace_back(m_rend);
}
void Render_Manager::Add(shared_ptr<Camera> mono)
{
	camera_list.emplace_back(mono);
}

void Render_Manager::Check_Renderer()
{
	shared_ptr<Renderer> p_rend = nullptr;
	//3Dオブジェクト
	{
		bool expired = false;
		for (auto& r : renderer_3D_list)
		{
			if (!r.expired())
			{
				p_rend = r.lock();
				p_rend->recalculated_frame = false;
			}
			else
			{
				expired = true;
			}
		}
		if (expired)
		{
			auto removeIt = remove_if(renderer_3D_list.begin(), renderer_3D_list.end(), [](weak_ptr<Renderer> r) { return r.expired(); });
			renderer_3D_list.erase(removeIt, renderer_3D_list.end());
		}
	}
	//2Dオブジェクト
	{
		bool expired = false;
		for (auto& r : renderer_2D_list)
		{
			if (!r.expired())
			{
				p_rend = r.lock();
				p_rend->recalculated_frame = false;
			}
			else
			{
				expired = true;
			}
		}
		if (expired)
		{
			auto removeIt = remove_if(renderer_2D_list.begin(), renderer_2D_list.end(), [](weak_ptr<Renderer> r) { return r.expired(); });
			renderer_2D_list.erase(removeIt, renderer_2D_list.end());
		}
	}
}

void Render_Manager::Render()
{
	Check_Renderer();

	shared_ptr<Camera> camera = nullptr;
	bool expired = false;
	for (auto& c : camera_list)
	{
		if (!c.expired())
		{
			camera = c.lock();
			if (camera->gameobject->Get_Active_In_Hierarchy())
			{
				if (camera->Get_Enabled())
				{
					// プロジェクション行列を作成
					float fov_y = XMConvertToRadians(camera->fov);	// 画角
					float aspect = (float)Engine::view_game->screen_x / (float)Engine::view_game->screen_y;	// 画面比率

					camera->projection_matrix = XMMatrixPerspectiveFovRH(fov_y, aspect, camera->near_z, camera->far_z);

					// ビュー行列を作成
					//{
					Vector3 eye_v = camera->transform->Get_Position();
					Vector3 focus_v = eye_v + camera->transform->Get_Forward();

					Vector3 camForward = XMVector3Normalize(focus_v - eye_v);    // Get forward vector based on target
					camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
					camForward = XMVector3Normalize(camForward);
					Vector3 camRight = { -XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward) };

					XMVECTOR up_v = camera->transform->Get_Up();
					camera->view_matrix = XMMatrixLookAtRH(eye_v, focus_v, up_v);
					//}

					Engine::particle_manager->Camera_Update(camera->transform, fov_y, camera->near_z, camera->far_z, aspect);
					Engine::view_game->Render(camera->view_matrix, camera->projection_matrix, camera->transform);
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
		auto removeIt = remove_if(camera_list.begin(), camera_list.end(), [](weak_ptr<Camera> m) { return m.expired(); });
		camera_list.erase(removeIt, camera_list.end());
	}
}