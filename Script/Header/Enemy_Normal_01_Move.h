#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_Mover.h"

namespace BeastEngine
{
	class Character_Parameter;
	class Enemy_Manager;

	class Enemy_Normal_01_Move final : public MonoBehaviour, public Interface_Character_Mover
	{
	public:
		void Move_Normal() override;
		void Move_Attack() override;
		void Move_Dodge() override;
		void Move_Damage() override;
		void Move_Guard() override;

		void Aerial_Update() override;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;
		std::weak_ptr<RigidBody> rigidbody;
		std::weak_ptr<Character_Parameter> parameter;

		float jump_power = 0;
		float down_power = 0;
		bool is_stay_air = false;
		bool is_add_down = false;
		Vector3 move_forward;

		// �V���A���C�Y�֐�
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), cereal::base_class<Interface_Character_Mover>(this), jump_power, down_power);
		}
	};
}

REGISTER_COMPONENT(Enemy_Normal_01_Move)
CEREAL_REGISTER_TYPE(BeastEngine::Enemy_Normal_01_Move)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Enemy_Normal_01_Move)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Interface_Character_Mover, BeastEngine::Enemy_Normal_01_Move)
CEREAL_CLASS_VERSION(BeastEngine::Enemy_Normal_01_Move, 1)