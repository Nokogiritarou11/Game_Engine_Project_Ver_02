#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Player_Camera_Controller;

	class Instant_Camera_Shake final : public MonoBehaviour
	{
		void Awake() override;
		void OnEnable() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Player_Camera_Controller> camera_controller;
		int shake_count = 0;
		float shake_power = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), shake_count, shake_power);
		}
	};
}

REGISTER_COMPONENT(Instant_Camera_Shake)
CEREAL_REGISTER_TYPE(BeastEngine::Instant_Camera_Shake)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Instant_Camera_Shake)
CEREAL_CLASS_VERSION(BeastEngine::Instant_Camera_Shake, 1)