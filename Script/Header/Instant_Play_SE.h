#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Instant_Play_SE final : public MonoBehaviour
	{
	private:
		void Awake() override;
		void OnEnable() override;
		bool Draw_ImGui() override;

		std::weak_ptr<AudioSource> source;
		float volume = 0;
		float pitch = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this));
		}
	};
}

REGISTER_COMPONENT(Instant_Play_SE)
CEREAL_REGISTER_TYPE(BeastEngine::Instant_Play_SE)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Instant_Play_SE)
CEREAL_CLASS_VERSION(BeastEngine::Instant_Play_SE, 1)