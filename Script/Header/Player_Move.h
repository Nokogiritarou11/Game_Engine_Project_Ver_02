#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Player_Move : public MonoBehaviour
	{
	public:
		float move_speed = 0;
		float run_speed = 0;
		float turn_speed = 0;

		void Move_Normal();
		void Move_Attack();
		void Move_Dodge();
		void Move_Damage();
		void Move_Guard();

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
			archive(cereal::base_class<BeastEngine::MonoBehaviour>(this), run_speed, turn_speed);
		}
	};
}

REGISTER_COMPONENT(Player_Move)
CEREAL_REGISTER_TYPE(BeastEngine::Player_Move)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_Move)
CEREAL_CLASS_VERSION(BeastEngine::Player_Move, 1)