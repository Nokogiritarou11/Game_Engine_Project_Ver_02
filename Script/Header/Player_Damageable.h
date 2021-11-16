#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_Damageable.h"

namespace BeastEngine
{
	class Character_Parameter;
	class Player_Camera_Controller;
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
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;
		std::weak_ptr<Character_Parameter> parameter;
		std::weak_ptr<Player_Camera_Controller> camera_controller;
		std::weak_ptr<Object_Pool> pool;
		std::weak_ptr<Character_Hit_Stop_Manager> hit_stop_manager;
		std::weak_ptr<Time_Manager> time_manager;

		float parry_time_stop_delay = 0;
		float parry_time_stop_time = 0;
		float parry_time_stop_speed = 0;

		int parry_shake_camera_count = 0;
		float parry_shake_camera_power = 0;

		Vector3 parry_particle_position;

		int guard_shake_camera_count = 0;
		float guard_shake_camera_power = 0;

		Vector3 guard_particle_position;

		std::string guard_particle_key;
		std::string parry_particle_key;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this),
				parry_time_stop_delay, parry_time_stop_time, parry_time_stop_speed,
				parry_particle_position,
				parry_shake_camera_count, parry_shake_camera_power,
				guard_shake_camera_count, guard_shake_camera_power,
				guard_particle_position,
				guard_particle_key, parry_particle_key);
		}
	};
}

REGISTER_COMPONENT(Player_Damageable)
CEREAL_REGISTER_TYPE(BeastEngine::Player_Damageable)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_Damageable)
CEREAL_CLASS_VERSION(BeastEngine::Player_Damageable, 1)