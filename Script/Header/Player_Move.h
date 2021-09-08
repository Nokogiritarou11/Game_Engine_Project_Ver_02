#pragma once
#include "MonoBehaviour.h"
#include "Character_Mover.h"

namespace BeastEngine
{
	class Player_Move : public MonoBehaviour, public Character_Mover
	{
	public:
		void Move_Normal() override;
		void Move_Attack() override;
		void Move_Dodge() override;
		void Move_Damage() override;
		void Move_Guard() override;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		void Check_Move_Direction();

		std::weak_ptr<Transform> camera_transform;
		std::weak_ptr<Animator> animator;
		std::weak_ptr<RigidBody> rigidbody;

		Vector3 camera_forward;
		Vector3 move_forward;



		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::MonoBehaviour>(this), cereal::base_class<BeastEngine::Character_Mover>(this));
		}
	};
}

REGISTER_COMPONENT(Player_Move)
CEREAL_REGISTER_TYPE(BeastEngine::Player_Move)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_Move)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Character_Mover, BeastEngine::Player_Move)
CEREAL_CLASS_VERSION(BeastEngine::Player_Move, 1)