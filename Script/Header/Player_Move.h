#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_Mover.h"

namespace BeastEngine
{
	class Character_Parameter;

	class Player_Move final : public MonoBehaviour, public Interface_Character_Mover
	{
	public:
		void Move_Normal() override;
		void Move_Attack() override;
		void Move_Dodge() override;
		void Move_Damage() override;
		void Move_Guard() override;

		void Jump() const;
		void Ground_Update() override;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		void Check_Move_Direction();

		std::weak_ptr<Transform> camera_transform;
		std::weak_ptr<Animator> animator;
		std::weak_ptr<RigidBody> rigidbody;
		std::weak_ptr<Character_Parameter> parameter;

		float jump_power = 0;
		float down_power = 0;
		bool is_stay_air = false;
		Vector3 camera_forward;
		Vector3 move_forward;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), cereal::base_class<Interface_Character_Mover>(this), jump_power, down_power);
		}
	};
}

REGISTER_COMPONENT(Player_Move)
CEREAL_REGISTER_TYPE(BeastEngine::Player_Move)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_Move)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Interface_Character_Mover, BeastEngine::Player_Move)
CEREAL_CLASS_VERSION(BeastEngine::Player_Move, 1)