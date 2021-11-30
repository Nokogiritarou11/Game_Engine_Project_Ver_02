#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_Damageable.h"

namespace BeastEngine
{
	class Player_Parameter;
	class Player_Camera_Controller;
	class Enemy_Manager;
	class Time_Manager;

	class Player_Damageable final : public MonoBehaviour, public Interface_Character_Damageable
	{
	public:
		bool Take_Damage(const std::shared_ptr<Damage_Collision>& damage_collision) override;

	private:
		struct Effect_Time_Stop_Parameter
		{
			float delay = 0;
			float time = 0;
			float speed = 0;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(delay, time, speed);
			}
		};

		struct Effect_Camera_Shake_Parameter
		{
			int count = 0;
			float power = 0;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(count, power);
			}
		};

		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Player_Parameter> parameter;
		std::weak_ptr<Player_Camera_Controller> camera_controller;
		std::weak_ptr<Time_Manager> time_manager;

		Effect_Time_Stop_Parameter parry_time_stop_parameter;
		Effect_Camera_Shake_Parameter parry_camera_shake_parameter;
		Effect_Camera_Shake_Parameter guard_camera_shake_parameter;

		Vector3 guard_particle_position;
		Vector3 parry_particle_position;

		std::string guard_particle_key;
		std::string parry_particle_key;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), cereal::base_class<Interface_Character_Damageable>(this),
				parry_time_stop_parameter, parry_camera_shake_parameter, parry_particle_position,
				guard_camera_shake_parameter, guard_particle_position,
				guard_particle_key, parry_particle_key);
		}
	};
}

REGISTER_COMPONENT(Player_Damageable)
CEREAL_REGISTER_TYPE(BeastEngine::Player_Damageable)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_Damageable)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Interface_Character_Damageable, BeastEngine::Player_Damageable)
CEREAL_CLASS_VERSION(BeastEngine::Player_Damageable, 1)