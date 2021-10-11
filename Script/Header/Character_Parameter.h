#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter final : public MonoBehaviour
	{
	public:
		float max_hp = 0;
		float hp = 0;

		bool eventing = false;
		bool pausing = false;
		bool living = true;
		bool moving = false;
		bool attacking = false;
		bool dodging = false;
		bool damaging = false;
		bool guarding = false;

		bool is_ground = true;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), max_hp);
		}
	};
}

REGISTER_COMPONENT(Character_Parameter)
CEREAL_REGISTER_TYPE(BeastEngine::Character_Parameter)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Character_Parameter)
CEREAL_CLASS_VERSION(BeastEngine::Character_Parameter, 1)