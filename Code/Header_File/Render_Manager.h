#pragma once
#include <memory>
#include <vector>
#include <list>
#include "SkinMesh_Renderer.h"
#include "Mesh_Renderer.h"
#include "Sprite_Renderer.h"

namespace BeastEngine
{
	class Camera;

	class Render_Manager
	{
	public:

		void Reset();
		void Add(std::shared_ptr<BeastEngine::Mesh_Renderer> m_rend);
		void Add(std::shared_ptr<BeastEngine::SkinMesh_Renderer> m_rend);
		void Add(std::shared_ptr<BeastEngine::Sprite_Renderer> m_rend);
		void Add(std::shared_ptr<BeastEngine::Camera> mono);
		void Render();

		std::vector<std::weak_ptr<BeastEngine::Renderer>> Renderer_3D_list;
		std::vector<std::weak_ptr<BeastEngine::Renderer>> Renderer_2D_list;
		std::vector<std::weak_ptr<BeastEngine::Camera>> Camera_list;

	private:

		struct Render_Obj
		{
			std::weak_ptr<BeastEngine::Renderer> renderer;
			int   Queue;
			float Z_Distance;
		};

		std::list<std::vector<Render_Obj>> Renderer_list;

		//Rendererの生存チェック
		void Check_Renderer();
	};
}