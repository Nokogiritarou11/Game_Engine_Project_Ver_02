#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Attack : public MonoBehaviour
	{
	public:

	private:
		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;
		std::vector<std::weak_ptr<BeastEngine::GameObject>> particle_list;
		std::vector<std::weak_ptr<BeastEngine::GameObject>> collider_list;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::MonoBehaviour>(this), particle_list, collider_list);
		}
	};
}

REGISTER_COMPONENT(Character_Attack)
CEREAL_REGISTER_TYPE(BeastEngine::Character_Attack)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Character_Attack)
CEREAL_CLASS_VERSION(BeastEngine::Character_Attack, 1)