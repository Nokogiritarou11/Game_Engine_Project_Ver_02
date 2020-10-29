#pragma once
#include "Mesh_Renderer.h"
#include "SkinMesh_Renderer.h"
#include "Sprite_Renderer.h"

class Render_Manager
{
public:

	void Reset();
	void Add(std::shared_ptr<Mesh_Renderer> m_rend);
	void Add(std::shared_ptr<SkinMesh_Renderer> m_rend);
	void Add(std::shared_ptr<Sprite_Renderer> m_rend);
	void Add(std::shared_ptr<Camera> mono);
	void Render();

	std::vector<std::weak_ptr<Renderer>> Renderer_3D_list;
	std::vector<std::weak_ptr<Renderer>> Renderer_2D_list;
	std::vector<std::weak_ptr<Camera>> Camera_list;

private:

	struct Render_Obj
	{
		std::weak_ptr<Renderer> renderer;
		int   Queue;
		float Z_Distance;
	};

	std::list<std::vector<Render_Obj>> Renderer_list;
};