#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_Damageable.h"

namespace BeastEngine
{
	class Character_Hit_Stop_Manager;
	class Object_Pool;

	class Damage_Collision final : public MonoBehaviour
	{
	public:

	private:
		void Awake() override;
		void OnTrigger_Enter(Collision& collision) override;
		bool Draw_ImGui() override;

		int damage_hp;
		int damage_stun;
		Damage_Type damage_type;

		std::weak_ptr<Character_Hit_Stop_Manager> hit_stop_manager;
		std::weak_ptr<Object_Pool> pool;
		std::weak_ptr<Transform> root_transform;
		std::weak_ptr<Transform> hit_transform;
		std::weak_ptr<Animator> animator;
		std::vector<std::weak_ptr<Particle>> stop_particle;
		std::string hit_particle_key;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			if (version <= 1)
			{
				archive(cereal::base_class<MonoBehaviour>(this), damage_hp, damage_stun, damage_type, root_transform, stop_particle);
			}
			else
			{
				archive(cereal::base_class<MonoBehaviour>(this), damage_hp, damage_stun, damage_type, root_transform, stop_particle, hit_particle_key);
			}
		}
	};
}

REGISTER_COMPONENT(Damage_Collision)
CEREAL_REGISTER_TYPE(BeastEngine::Damage_Collision)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Damage_Collision)
CEREAL_CLASS_VERSION(BeastEngine::Damage_Collision, 2)