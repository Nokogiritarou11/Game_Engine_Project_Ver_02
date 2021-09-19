#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Disactive_Particle : public MonoBehaviour
	{
	public:

	private:
		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<BeastEngine::Particle> particle;
		bool remove_parent;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::MonoBehaviour>(this), remove_parent);
		}
	};
}

REGISTER_COMPONENT(Disactive_Particle)
CEREAL_REGISTER_TYPE(BeastEngine::Disactive_Particle)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Disactive_Particle)
CEREAL_CLASS_VERSION(BeastEngine::Disactive_Particle, 1)