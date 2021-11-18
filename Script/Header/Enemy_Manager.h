#pragma once
#include "MonoBehaviour.h"
#include "Character_Parameter.h"

namespace BeastEngine
{
	class Player_Camera_Controller;
	class Time_Manager;
	class Object_Pool;

	class Enemy_Manager final : public MonoBehaviour
	{
	public:
		void Instance_Enemy(const Character_Type& type, const Vector3& position, const Quaternion& rotation);

		void Add_Attacking_List(const std::weak_ptr<Character_Parameter>& parameter);
		void Remove_Attacking_List(const std::weak_ptr<Character_Parameter>& parameter);

		void Remove_Stunning_List(const std::weak_ptr<Character_Parameter>& parameter);

		void Enemy_Dead(const bool& use_effect, const std::weak_ptr<Character_Parameter>& parameter);
		void Enemy_Stunned(const bool& use_effect, const std::weak_ptr<Character_Parameter>& parameter);

		Vector3 Get_Nearest_Enemy_Position(const Vector3& position);

		std::vector<std::weak_ptr<Character_Parameter>> enemy_list{};
		std::vector<std::weak_ptr<Character_Parameter>> attacking_list{};
		std::vector<std::weak_ptr<Character_Parameter>> stunning_list{};

		std::weak_ptr<Character_Parameter> last_attack_target;

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

		struct Effect_Parameter
		{
			Effect_Time_Stop_Parameter time_parameter;
			Effect_Camera_Shake_Parameter shake_parameter;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(time_parameter, shake_parameter);
			}
		};

		void Awake() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Character_Parameter> player_parameter;
		std::weak_ptr<Player_Camera_Controller> camera_controller;
		std::weak_ptr<Time_Manager> time_manager;
		std::weak_ptr<Object_Pool> pool;

		Effect_Parameter effect_dead;
		Effect_Parameter effect_stun;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), effect_dead, effect_stun);
		}
	};
}

REGISTER_COMPONENT(Enemy_Manager)
CEREAL_REGISTER_TYPE(BeastEngine::Enemy_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Enemy_Manager)
CEREAL_CLASS_VERSION(BeastEngine::Enemy_Manager, 1)