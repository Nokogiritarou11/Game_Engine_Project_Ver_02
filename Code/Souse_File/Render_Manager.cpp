#include "Render_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Engine.h"
using namespace std;

list<weak_ptr<Renderer>> Render_Manager::Renderer_3D_list;
list<weak_ptr<Renderer>> Render_Manager::Renderer_2D_list;
list<vector<Render_Manager::Render_Obj>> Render_Manager::Renderer_list;
list<weak_ptr<Camera>> Render_Manager::Camera_list;

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
			if (camera->enabled)
			{
				camera->Update(Engine::view_game->screen_x, Engine::view_game->screen_y);
				Engine::view_game->Render(camera->V, camera->P);
			}
		}
		itr++;
	}
}