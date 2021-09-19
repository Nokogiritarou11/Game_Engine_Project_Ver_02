#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_Damageable.h"

namespace BeastEngine
{
	class Character_Parameter;

	class Enemy_Damageable_Normal : public MonoBehaviour, public Interface_Character_Damageable
	{
	public:
		void Take_Damage(int damage_hp, int damage_stun, Vector3 from_pos, Damage_Type damage_state) override;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;
		std::weak_ptr<Character_Parameter> parameter;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::MonoBehaviour>(this));
		}
	};
}

REGISTER_COMPONENT(Enemy_Damageable_Normal)
CEREAL_REGISTER_TYPE(BeastEngine::Enemy_Damageable_Normal)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Enemy_Damageable_Normal)
CEREAL_CLASS_VERSION(BeastEngine::Enemy_Damageable_Normal, 1)