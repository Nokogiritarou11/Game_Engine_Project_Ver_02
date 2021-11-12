#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter;
	class Player_Camera_Controller;
	class Time_Manager;

	class Enemy_Manager final : public MonoBehaviour
	{
	public:
		void Add_Enemy_List(const std::weak_ptr<Character_Parameter>& parameter);

		void Add_Attacking_List(const std::weak_ptr<Character_Parameter>& parameter);
		void Remove_Attacking_List(const std::weak_ptr<Character_Parameter>& parameter);

		void Remove_Stunning_List(const std::weak_ptr<Character_Parameter>& parameter);

		void Enemy_Dead(const std::weak_ptr<Character_Parameter>& parameter);
		void Enemy_Stunned(const std::weak_ptr<Character_Parameter>& parameter);

		Vector3 Get_Nearest_Enemy_Position(const Vector3& position);

		std::vector<std::weak_ptr<Character_Parameter>> enemy_list{};
		std::vector<std::weak_ptr<Character_Parameter>> attacking_list{};
		std::vector<std::weak_ptr<Character_Parameter>> stunning_list{};

		std::weak_ptr<Character_Parameter> last_attack_target;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Player_Camera_Controller> camera_controller;
		std::weak_ptr<Time_Manager> time_manager;

		float dead_time_stop_delay = 0;
		float dead_time_stop_time = 0;
		float dead_time_stop_speed = 0;

		int dead_shake_camera_count = 0;
		float dead_shake_camera_power = 0;

		float stun_time_stop_delay = 0;
		float stun_time_stop_time = 0;
		float stun_time_stop_speed = 0;

		int stun_shake_camera_count = 0;
		float stun_shake_camera_power = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this),
				dead_time_stop_delay, dead_time_stop_time, dead_time_stop_speed,
				dead_shake_camera_count, dead_shake_camera_power,
				stun_time_stop_delay, stun_time_stop_time, stun_time_stop_speed,
				stun_shake_camera_count, stun_shake_camera_power);
		}
	};
}

REGISTER_COMPONENT(Enemy_Manager)
CEREAL_REGISTER_TYPE(BeastEngine::Enemy_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Enemy_Manager)
CEREAL_CLASS_VERSION(BeastEngine::Enemy_Manager, 1)