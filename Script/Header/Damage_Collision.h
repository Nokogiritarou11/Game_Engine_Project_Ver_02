#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_Damageable.h"

namespace BeastEngine
{
	class Character_Hit_Stop_Manager;

	class Damage_Collision final : public MonoBehaviour
	{
	public:
		int damage_hp;
		int damage_stun;

		Damage_Type damage_type;
		bool can_parry = true;

		std::weak_ptr<Transform> root_transform;
		std::weak_ptr<Transform> hit_transform;

		float hit_stop_time = 0;
		int shake_count = 0;
		float shake_power = 0;
		std::string hit_particle_key;

	private:
		void Awake() override;
		void OnTrigger_Enter(Collision& collision) override;
		bool Draw_ImGui() override;

		std::weak_ptr<Character_Hit_Stop_Manager> hit_stop_manager;
		std::weak_ptr<Animator> animator;
		std::vector<std::weak_ptr<Particle>> stop_particle;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this),
				damage_hp, damage_stun, damage_type,
				can_parry, root_transform, stop_particle,
				hit_stop_time, shake_count, shake_power,
				hit_particle_key);
		}
	};
}

REGISTER_COMPONENT(Damage_Collision)
CEREAL_REGISTER_TYPE(BeastEngine::Damage_Collision)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Damage_Collision)
CEREAL_CLASS_VERSION(BeastEngine::Damage_Collision, 1)