#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Disable_Particle : public MonoBehaviour
	{
	private:
		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Particle> particle;
		bool remove_parent;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), remove_parent);
		}
	};
}

REGISTER_COMPONENT(Disable_Particle)
CEREAL_REGISTER_TYPE(BeastEngine::Disable_Particle)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Disable_Particle)
CEREAL_CLASS_VERSION(BeastEngine::Disable_Particle, 1)