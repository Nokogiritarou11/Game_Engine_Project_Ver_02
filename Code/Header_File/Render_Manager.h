#pragma once
#include <memory>
#include <vector>
#include <list>

namespace BeastEngine
{
	class Camera;
	class Mesh_Renderer;
	class SkinMesh_Renderer;
	class Sprite_Renderer;
	class Renderer;

	class Render_Manager
	{
	public:

		void Reset();
		void Add(std::shared_ptr<BeastEngine::Mesh_Renderer> m_rend);
		void Add(std::shared_ptr<BeastEngine::SkinMesh_Renderer> m_rend);
		void Add(std::shared_ptr<BeastEngine::Sprite_Renderer> m_rend);
		void Add(std::shared_ptr<BeastEngine::Camera> mono);
		void Render();

		std::vector<std::weak_ptr<BeastEngine::Renderer>> renderer_3D_list;
		std::vector<std::weak_ptr<BeastEngine::Renderer>> renderer_2D_list;
		std::vector<std::weak_ptr<BeastEngine::Camera>> camera_list;

	private:

		struct Render_Obj
		{
			std::weak_ptr<BeastEngine::Renderer> renderer;
			int   queue;
			float Z_distance;
		};

		std::list<std::vector<Render_Obj>> renderer_list;

		//Rendererの生存チェック
		void Check_Renderer();
	};
}