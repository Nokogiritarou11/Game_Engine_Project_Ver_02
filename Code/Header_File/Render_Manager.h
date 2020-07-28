#pragma once
#include "Mesh_Renderer.h"
#include "SkinMesh_Renderer.h"
#include "Sprite_Renderer.h"

class Render_Manager
{
public:

	static void Reset();
	static void Add(std::shared_ptr<Mesh_Renderer> m_rend);
	static void Add(std::shared_ptr<SkinMesh_Renderer> m_rend);
	static void Add(std::shared_ptr<Sprite_Renderer> m_rend);
	static void Add(std::shared_ptr<Camera> mono);
	static void Update();
	static void Render();

	static std::list<std::weak_ptr<Renderer>> Renderer_3D_list;
	static std::list<std::weak_ptr<Renderer>> Renderer_2D_list;
	static std::list<std::weak_ptr<Camera>> Camera_list;

private:

	struct Render_Obj
	{
		std::weak_ptr<Renderer> renderer;
		int   Queue;
		float Z_Distance;
	};

	static std::list<std::vector<Render_Obj>> Renderer_list;
};