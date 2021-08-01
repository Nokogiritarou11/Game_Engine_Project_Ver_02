#pragma once
#include <memory>
#include <vector>
#include <list>
#include "DxSystem.h"
#include "Original_Math.h"

namespace BeastEngine
{
	class Transform;
	class Camera;
	class Mesh_Renderer;
	class SkinMesh_Renderer;
	class Sprite_Renderer;
	class Renderer;
	class Render_Texture;
	class SkyBox;

	class Render_Manager
	{
	public:
		Render_Manager();

		void Reset();
		void Add(std::shared_ptr<BeastEngine::Mesh_Renderer> m_rend);
		void Add(std::shared_ptr<BeastEngine::SkinMesh_Renderer> m_rend);
		void Add(std::shared_ptr<BeastEngine::Sprite_Renderer> m_rend);
		void Add(std::shared_ptr<BeastEngine::Camera> mono);
		void Render();

		std::unique_ptr<BeastEngine::Render_Texture> scene_texture;
		std::unique_ptr<BeastEngine::Render_Texture> game_texture;
	private:
		struct Render_Obj
		{
			std::weak_ptr<BeastEngine::Renderer> renderer;
			int   queue;
			float Z_distance;
		};

		std::unique_ptr<BeastEngine::SkyBox> skybox;
		std::list<std::vector<Render_Obj>> renderer_list;
		std::vector<std::weak_ptr<BeastEngine::Renderer>> renderer_3D_list;
		std::vector<std::weak_ptr<BeastEngine::Renderer>> renderer_2D_list;
		std::vector<std::weak_ptr<BeastEngine::Camera>> camera_list;

		struct Constant_Buffer_Scene
		{
			BeastEngine::Matrix	 view_projection_matrix;
			BeastEngine::Matrix	 shadow_matrix;
			BeastEngine::Vector4 light_direction;
			BeastEngine::Vector3 light_color;
			float	bias;
		};
		Constant_Buffer_Scene buffer_scene;
		Microsoft::WRL::ComPtr <ID3D11Buffer> constant_buffer_scene; //コンスタントバッファ

		//Rendererの生存チェック
		void Check_Renderer();

		void Render_Scene();
		void Render_Game();

		void Render_Sky(BeastEngine::Vector3& pos);
		void Render_Shadow(std::shared_ptr<BeastEngine::Transform>& camera_transform);
		void Render_Shadow_Directional(BeastEngine::Vector4 color, float intensity, std::shared_ptr<BeastEngine::Transform>& light_transform, std::shared_ptr<BeastEngine::Transform>& camera_transform);
		void Render_3D();
		void Render_2D();
	};
}