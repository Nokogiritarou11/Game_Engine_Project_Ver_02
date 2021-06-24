#pragma once
#include "Behaviour.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>

namespace BeastEngine
{
	class Camera : public BeastEngine::Behaviour
	{
	public:

		float fov = 30.0f;
		float near_z = 0.1f;	// 表示最近面までの距離
		float far_z = 1000.0f;	// 表示最遠面までの距離

		BeastEngine::Matrix view_matrix = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		BeastEngine::Matrix projection_matrix = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

		BeastEngine::Vector2 World_To_ViewportPoint(BeastEngine::Vector3 pos);

	private:

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Behaviour>(this), fov, near_z, far_z);
		}
		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return false; };

		D3D11_VIEWPORT viewport;
		UINT num_viewports = 1;
	};
}
CEREAL_REGISTER_TYPE(BeastEngine::Camera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::Camera)