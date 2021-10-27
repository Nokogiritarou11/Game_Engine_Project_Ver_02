#pragma once
#include <memory>
#include <vector>
#include <array>
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
	class Material;

	class Render_Manager
	{
	public:
		Render_Manager();

		void Reset();
		void Add(const std::shared_ptr<Mesh_Renderer>& m_rend);
		void Add(const std::shared_ptr<SkinMesh_Renderer>& m_rend);
		void Add(const std::shared_ptr<Sprite_Renderer>& m_rend);
		void Add(const std::shared_ptr<Camera>& mono);
		void Render();

		bool render_game = true;
		bool render_scene = false;
		std::shared_ptr<Render_Texture> scene_texture{};
		std::shared_ptr<Render_Texture> game_texture{};
		std::shared_ptr<Render_Texture> staging_texture{};

		std::shared_ptr<Material> sprite_material;

	private:
		struct Render_Obj
		{
			std::weak_ptr<Renderer> renderer;
			int subset_number{};
			int queue{};
			float z_distance{};
		};

		std::unique_ptr<SkyBox> skybox{};
		std::shared_ptr<Transform> shadow_camera_transform;
		std::shared_ptr<Camera> shadow_camera;

		std::vector<Render_Obj> opaque_list{};
		std::vector<Render_Obj> alpha_list{};
		std::vector<std::weak_ptr<Renderer>> renderer_3D_list{};
		std::vector<std::weak_ptr<Renderer>> renderer_2D_list{};
		std::vector<std::weak_ptr<Camera>> camera_list{};

		struct Constant_Buffer_Scene
		{
			Matrix	 view_projection_matrix;
			Matrix	 shadow_matrix;
			Vector4 camera_direction;
			Vector4 camera_position;
			Vector4 light_direction;
			Vector3 light_color;
			float	bias{};
		};
		Constant_Buffer_Scene buffer_scene;
		Microsoft::WRL::ComPtr <ID3D11Buffer> constant_buffer_scene; //コンスタントバッファ

		//Rendererの生存チェック
		void Check_Renderer();
		void Culling_Renderer(const Vector3& view_pos, const std::array<Vector4, 6>& planes);
		void Sort_Renderer();

		void Render_Scene();
		void Render_Game();

		void Render_Sky(const Vector3& pos) const;
		void Render_Shadow(const std::shared_ptr<Transform>& camera_transform);
		void Render_Shadow_Directional(const Vector3& color, const float& intensity, const std::shared_ptr<Transform>& light_transform, const std::shared_ptr<Transform>& camera_transform);
		void Render_3D(const std::shared_ptr<Camera>& camera);
		void Render_2D();

		void Update_Constant_Buffer() const;
	};
}