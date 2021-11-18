#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	enum class Character_Type
	{
		Player,
		Enemy_Normal_01,
		Enemy_Big_01,
		Boss_01
	};

	class Character_Parameter final : public MonoBehaviour
	{
	public:
		Character_Type type;

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
		void OnEnable() override;
		bool Draw_ImGui() override;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			if (version <= 1)
			{
				archive(cereal::base_class<MonoBehaviour>(this), max_hp, max_stun, heal_stun);
			}
			else
			{
				archive(cereal::base_class<MonoBehaviour>(this), type, max_hp, max_stun, heal_stun);
			}
		}
	};
}

REGISTER_COMPONENT(Character_Parameter)
CEREAL_REGISTER_TYPE(BeastEngine::Character_Parameter)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Character_Parameter)
CEREAL_CLASS_VERSION(BeastEngine::Character_Parameter, 2)