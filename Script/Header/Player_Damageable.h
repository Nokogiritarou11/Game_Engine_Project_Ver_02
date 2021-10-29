#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_Damageable.h"

namespace BeastEngine
{
	class Character_Parameter;
	class Enemy_Manager;
	class Character_Hit_Stop_Manager;
	class Time_Manager;
	class Object_Pool;

	class Player_Damageable final : public MonoBehaviour, public Interface_Character_Damageable
	{
	public:
		bool Take_Damage(int damage_hp, int damage_stun, const std::shared_ptr<Transform>& from_transform, Damage_Type damage_state) override;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;
		std::weak_ptr<Character_Parameter> parameter;
		std::weak_ptr<Object_Pool> pool;
		std::weak_ptr<Character_Hit_Stop_Manager> hit_stop_manager;
		std::weak_ptr<Time_Manager> time_manager;

		std::string guard_particle_key;

		// �V���A���C�Y�֐�
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), guard_particle_key);
		}
	};
}

REGISTER_COMPONENT(Player_Damageable)
CEREAL_REGISTER_TYPE(BeastEngine::Player_Damageable)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_Damageable)
CEREAL_CLASS_VERSION(BeastEngine::Player_Damageable, 1)