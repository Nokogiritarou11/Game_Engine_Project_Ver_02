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

		void Reset(); //リセット
		void Add(const std::shared_ptr<Mesh_Renderer>& m_rend);     //マネージャーにレンダラーを登録する
		void Add(const std::shared_ptr<SkinMesh_Renderer>& m_rend); //マネージャーにレンダラーを登録する
		void Add(const std::shared_ptr<Sprite_Renderer>& m_rend);   //マネージャーにレンダラーを登録する
		void Add(const std::shared_ptr<Camera>& mono);              //マネージャーにレンダラーを登録する
		void Render(); //描画実行

		bool render_game = true;   //ゲームビューを描画するか
		bool render_scene = false; //シーンビューを描画するか
		std::shared_ptr<Render_Texture> scene_texture{}; //シーンビューのテクスチャ
		std::shared_ptr<Render_Texture> game_texture{};  //ゲームビューのテクスチャ
		std::shared_ptr<Render_Texture> staging_texture{}; //ステージにバインド中のテクスチャ

		std::shared_ptr<Material> sprite_material;

	private:
		//レンダリング時に必要なデータ
		struct Render_Obj
		{
			std::weak_ptr<Renderer> renderer;
			int subset_number = 0;
			int queue = 0;
			float z_distance = 0;
		};

		std::unique_ptr<SkyBox> skybox{};
		std::shared_ptr<Transform> shadow_camera_transform;
		std::shared_ptr<Camera> shadow_camera;

		std::vector<Render_Obj> opaque_list{}; //不透明なレンダラー
		std::vector<Render_Obj> alpha_list{};  //半透明なレンダラー

		std::vector<std::weak_ptr<Renderer>> renderer_3D_list{};
		std::vector<std::weak_ptr<Renderer>> renderer_2D_list{};
		std::vector<std::weak_ptr<Camera>> camera_list{};

		//レンダリング共通コンスタントバッファ
		struct Constant_Buffer_Scene
		{
			Matrix	view_projection_matrix;
			Matrix	shadow_matrix;
			Vector4 camera_direction;
			Vector4 camera_position;
			Vector4 light_direction;
			Vector3 light_color;
			float	bias = 0;
		};
		Constant_Buffer_Scene buffer_scene;
		Microsoft::WRL::ComPtr <ID3D11Buffer> constant_buffer_scene;

		void Check_Renderer(); //Rendererの生存をチェックする
		void Culling_Renderer(const Vector3& view_pos, const std::array<Vector4, 6>& planes); //視錐台カリングを行う
		void Sort_Renderer(); //レンダラーを距離&キューソートする

		void Render_Scene(); //シーンビューの描画実行
		void Render_Game();  //ゲームビューの描画実行

		void Render_Sky(const Vector3& pos) const; //スカイボックスの描画実行
		void Render_Shadow(const std::shared_ptr<Transform>& camera_transform); //シャドウマップの描画実行
		void Render_Shadow_Directional(const Vector3& color, const float& intensity, const std::shared_ptr<Transform>& light_transform, const std::shared_ptr<Transform>& camera_transform); //ディレクショナルライトの描画実行
		void Render_3D(const std::shared_ptr<Camera>& camera); //3Dオブジェクトのレンダリング
		void Render_2D(); //2Dオブジェクトのレンダリング

		void Update_Constant_Buffer() const; //コンスタントバッファの更新
	};
}