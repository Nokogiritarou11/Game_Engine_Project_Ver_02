#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter final : public MonoBehaviour
	{
	public:
		float max_hp = 0;
		float hp = 0;
		float max_stun = 0;
		float stun = 0;
		float heal_stun = 0;

		bool eventing = false;
		bool pausing = false;
		bool living = true;
		bool moving = false;
		bool attacking = false;
		bool damaging = false;
		bool guarding = false;
		bool just_guarding = false;
		bool stunning = false;
		bool camera_locking = false;

		bool is_ground = true;
		bool is_attack_preliminary = false;
		bool is_invincible = false;

		std::weak_ptr<Character_Parameter> target;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), max_hp, max_stun, heal_stun);
		}
	};
}

REGISTER_COMPONENT(Character_Parameter)
CEREAL_REGISTER_TYPE(BeastEngine::Character_Parameter)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Character_Parameter)
CEREAL_CLASS_VERSION(BeastEngine::Character_Parameter, 1)