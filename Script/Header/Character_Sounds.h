#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Sounds final : public MonoBehaviour
	{
	private:

		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;
		std::vector<std::weak_ptr<AudioSource>> sound_list{};

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), sound_list);
		}
	};
}

REGISTER_COMPONENT(Character_Sounds)
CEREAL_REGISTER_TYPE(BeastEngine::Character_Sounds)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Character_Sounds)
CEREAL_CLASS_VERSION(BeastEngine::Character_Sounds, 1)