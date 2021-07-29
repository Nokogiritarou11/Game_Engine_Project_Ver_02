#pragma once
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include <memory>
#include "Original_Math.h"

namespace BeastEngine
{
	class Transform;
	class Collider;
	class Box_Collider;

	enum class Force_Mode
	{
		Force,
		Impulse
	};

	class RigidBody
	{
	public:
		~RigidBody();

		void Set_Mass(float set_mass);
		float Get_Mass() const;

		void Set_Kinematic(bool value);
		void Set_Dynamic(bool value);
		void Use_Gravity(bool value);

		void Set_Freeze_Position(bool x_axis, bool y_axis, bool z_axis);
		void Set_Freeze_Rotation(bool x_axis, bool y_axis, bool z_axis);

		void Set_Angular_Velocity(BeastEngine::Vector3 velocity);
		BeastEngine::Vector3 Get_Angular_Velocity() const;
		void Set_Velocity(BeastEngine::Vector3 velocity);
		BeastEngine::Vector3 Get_Velocity() const;

		void Add_Force(BeastEngine::Vector3 force, Force_Mode mode = Force_Mode::Force);
		void Add_Force_AtPosition(BeastEngine::Vector3 force, BeastEngine::Vector3 position, Force_Mode mode = Force_Mode::Force);

	private:
		void Initialize(std::shared_ptr<BeastEngine::Collider> col);
		void Create();
		void Resize();
		void Remove();
		void Get_btTransform(btTransform& t);
		void Set_btTransform(btTransform& t);
		void Set_Debug_Draw(bool value);

		std::unique_ptr<btRigidBody> rigidbody;
		std::unique_ptr<btDefaultMotionState> motion_state;
		std::weak_ptr<BeastEngine::Collider> collider;

		float mass = 1.0f;
		float send_mass = 1.0f;
		bool is_dynamic = true;
		bool is_kinematic = false;
		BeastEngine::Vector3 gravity = { 0.0f, -9.8f, 0.0f };
		BeastEngine::Vector3 linear_factor = { 1.0f, 1.0f, 1.0f };
		BeastEngine::Vector3 angular_factor = { 1.0f, 1.0f, 1.0f };

		friend class BeastEngine::Collider;
		friend class BeastEngine::Box_Collider;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(mass, is_dynamic, is_kinematic, gravity, linear_factor, angular_factor);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::RigidBody, 1)