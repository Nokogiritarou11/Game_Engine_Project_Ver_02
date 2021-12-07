#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_Mover.h"

namespace BeastEngine
{
	class Enemy_Parameter;

	class Enemy_Boss_01_Move final : public MonoBehaviour, public Interface_Character_Mover
	{
	public:
		void Move_Normal() override;
		void Move_Attack() override;
		void Move_Damage() override;
		void Move_Guard() override;

		void Move_Update() override;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		void Attack_Melee() const;
		void Attack_Dash();
		void Attack_Jump();

		std::weak_ptr<Animator> animator;
		std::weak_ptr<Capsule_Collider> collider;
		std::weak_ptr<RigidBody> rigidbody;
		std::weak_ptr<Enemy_Parameter> parameter;
		std::weak_ptr<Transform> target_transform;

		float jump_power = 0;
		float down_power = 0;
		bool is_stay_air = false;

		Vector3 dash_forward;
		Vector3 jump_start_position;
		float jump_attack_rate_timer = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), cereal::base_class<Interface_Character_Mover>(this), jump_power, down_power);
		}
	};
}

REGISTER_COMPONENT(Enemy_Boss_01_Move)
CEREAL_REGISTER_TYPE(BeastEngine::Enemy_Boss_01_Move)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Enemy_Boss_01_Move)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Interface_Character_Mover, BeastEngine::Enemy_Boss_01_Move)
CEREAL_CLASS_VERSION(BeastEngine::Enemy_Boss_01_Move, 1)