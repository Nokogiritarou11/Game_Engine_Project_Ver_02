#pragma once
#include <memory>
#include <array>
#include <d3d11.h>
#include <Original_Math.h>
#include "Behaviour.h"

namespace BeastEngine
{
	class Render_Manager;
	class Editor;

	class Camera final : public Behaviour
	{
	public:
		[[nodiscard]] Vector2 World_To_Screen_Point(Vector3 pos) const; //ワールド座標を画面上の座標に変換する
		[[nodiscard]] Vector2 World_To_Viewport_Point(Vector3 pos) const; //ワールド座標を画面上の座標に変換する

		float fov = 60.0f;                 //視野角
		float near_z = 0.1f;	           // 表示最近面までの距離
		float far_z = 1000.0f;	           // 表示最遠面までの距離
		float orthographic_size = 1000.0f; //平行投影時の描画範囲
		bool is_orthographic = false;      //平行投影か
		Matrix view_matrix = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		Matrix projection_matrix = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		Matrix view_projection_matrix = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		std::array<Vector4, 6> frustum_planes; //視錐台カリング用Plane

	private:
		void Initialize(const std::shared_ptr<GameObject>& obj) override; //初期化
		bool Draw_ImGui() override;                                       //ImGui描画
		bool Can_Multiple() override { return false; };                   //同コンポーネントを複数アタッチ可能か
		void Update(float screen_x, float screen_y);                      //更新

		D3D11_VIEWPORT viewport;
		UINT num_viewports = 1;

		friend class Render_Manager;
		friend class Editor;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Behaviour>(this), fov, near_z, far_z, orthographic_size, is_orthographic);
		}
	};
}

REGISTER_COMPONENT(Camera)
CEREAL_REGISTER_TYPE(BeastEngine::Camera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::Camera)
CEREAL_CLASS_VERSION(BeastEngine::Camera, 1)