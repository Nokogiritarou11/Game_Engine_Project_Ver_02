#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_State_Setter.h"

namespace BeastEngine
{
	class Enemy_Parameter;
	class Enemy_Manager;
	class Object_Pool;

	class Enemy_Boss_01_State_Setter final : public MonoBehaviour, public Interface_Character_State_Setter
	{
	public:
		void Set_State() override;

	private:
		enum class Action_State
		{
			Preparation,
			Attack,
		};
		enum class Preparation_State
		{
			Chase,
			Leave,
		};
		enum class Attack_State
		{
			Melee_3_Combo,
			Melee_Once,
			Slash_Dash,
			Slash_Jump,
		};

		struct Attack_Action
		{
			Preparation_State preparation_state = Preparation_State::Chase;
			float preparation_distance_min = 0;
			float preparation_distance_max = 0;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive, std::uint32_t const version)
			{
				archive(preparation_state, preparation_distance_min, preparation_distance_max);
			}
		};

		void Change_Attack_State();

		void Awake() override;
		void OnEnable() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;
		std::weak_ptr<Transform> target_transform;
		std::weak_ptr<Transform> spine_transform;
		std::weak_ptr<GameObject> flash_particle;
		std::weak_ptr<Enemy_Parameter> parameter;
		std::weak_ptr<Enemy_Manager> enemy_manager;
		std::weak_ptr<Object_Pool> pool;

		std::vector<Attack_Action> attack_action{};

		Action_State action_state = Action_State::Preparation;
		Preparation_State preparation_state = Preparation_State::Chase;
		Attack_State attack_state = Attack_State::Melee_3_Combo;
		float preparation_distance = 0;
		float melee_probability = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), spine_transform, flash_particle, attack_action, melee_probability);
		}
	};
}

REGISTER_COMPONENT(Enemy_Boss_01_State_Setter)
CEREAL_REGISTER_TYPE(BeastEngine::Enemy_Boss_01_State_Setter)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Enemy_Boss_01_State_Setter)
CEREAL_CLASS_VERSION(BeastEngine::Enemy_Boss_01_State_Setter, 1)