#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter;

	class Character_Ground_Checker final : public MonoBehaviour
	{
	public:

	private:
		void Awake() override;
		void OnEnable() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Character_Parameter> parameter;
		std::weak_ptr<Animator> animator;
		float ray_distance = 0;
		bool ground_old = false;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), ray_distance);
		}
	};
}

REGISTER_COMPONENT(Character_Ground_Checker)
CEREAL_REGISTER_TYPE(BeastEngine::Character_Ground_Checker)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Character_Ground_Checker)
CEREAL_CLASS_VERSION(BeastEngine::Character_Ground_Checker, 1)