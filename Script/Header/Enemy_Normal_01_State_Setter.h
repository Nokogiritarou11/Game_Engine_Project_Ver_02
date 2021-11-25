#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_State_Setter.h"

namespace BeastEngine
{
	class Enemy_Parameter;
	class Enemy_Manager;
	class Object_Pool;

	class Enemy_Normal_01_State_Setter final : public MonoBehaviour, public Interface_Character_State_Setter
	{
	public:
		void Set_State() override;

	private:
		enum class Wait_State
		{
			Wait,
			Chase,
			Right,
			Left
		};

		void Change_Wait_State();

		void Awake() override;
		void OnEnable() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;
		std::weak_ptr<Transform> target_transform;
		std::weak_ptr<Transform> spine_transform;
		std::weak_ptr<Enemy_Parameter> parameter;
		std::weak_ptr<Enemy_Manager> enemy_manager;
		std::weak_ptr<Object_Pool> pool;

		Wait_State wait_state = Wait_State::Wait;
		bool attack_mode_old = false;

		float wait_state_timer = 0;
		float state_change_time_min = 0;
		float state_change_time_max = 0;
		float attack_distance = 0;
		float wait_distance = 0;
		float wait_distance_min = 0;
		float wait_distance_max = 0;

		float probability_wait = 0;
		float probability_chase = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this),
				spine_transform,
				attack_distance,
				state_change_time_min, state_change_time_max,
				probability_wait, probability_chase,
				wait_distance_min, wait_distance_max);
		}
	};
}

REGISTER_COMPONENT(Enemy_Normal_01_State_Setter)
CEREAL_REGISTER_TYPE(BeastEngine::Enemy_Normal_01_State_Setter)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Enemy_Normal_01_State_Setter)
CEREAL_CLASS_VERSION(BeastEngine::Enemy_Normal_01_State_Setter, 1)