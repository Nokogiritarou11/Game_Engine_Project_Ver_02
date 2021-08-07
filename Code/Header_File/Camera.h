#pragma once
#include <memory>
#include <array>
#include <d3d11.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include "Behaviour.h"

namespace BeastEngine
{
	class Render_Manager;
	class Editor;

	class Camera : public BeastEngine::Behaviour
	{
	public:
		BeastEngine::Vector2 World_To_ViewportPoint(BeastEngine::Vector3 pos);

		float fov = 60.0f;
		float near_z = 0.1f;	// ï\é¶ç≈ãﬂñ Ç‹Ç≈ÇÃãóó£
		float far_z = 1000.0f;	// ï\é¶ç≈âìñ Ç‹Ç≈ÇÃãóó£
		float orthographic_size = 1000.0f;
		bool is_orthographic = false;
		BeastEngine::Matrix view_matrix = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		BeastEngine::Matrix projection_matrix = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		BeastEngine::Matrix view_projection_matrix = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		std::array<BeastEngine::Vector4, 6> frustum_planes;

	private:
		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return false; };
		void Update(float screen_x, float screen_y);

		D3D11_VIEWPORT viewport;
		UINT num_viewports = 1;

		friend class BeastEngine::Render_Manager;
		friend class BeastEngine::Editor;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Behaviour>(this), fov, near_z, far_z, orthographic_size, is_orthographic);
		}
	};
}

REGISTER_COMPONENT(Camera)
CEREAL_REGISTER_TYPE(BeastEngine::Camera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::Camera)
CEREAL_CLASS_VERSION(BeastEngine::Camera, 1)