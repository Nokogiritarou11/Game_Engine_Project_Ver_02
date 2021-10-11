#pragma once
#include "btBulletDynamicsCommon.h"
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
		[[nodiscard]] float Get_Mass() const;

		void Set_Kinematic(bool value);
		void Set_Dynamic(bool value);
		void Use_Gravity(bool value);

		void Set_Freeze_Position(bool x_axis, bool y_axis, bool z_axis);
		void Set_Freeze_Rotation(bool x_axis, bool y_axis, bool z_axis);

		void Set_Angular_Velocity(Vector3 velocity) const;
		[[nodiscard]] Vector3 Get_Angular_Velocity() const;
		void Set_Velocity(Vector3 velocity) const;
		[[nodiscard]] Vector3 Get_Velocity() const;

		void Add_Force(Vector3 force, Force_Mode mode = Force_Mode::Force) const;
		void Add_Force_AtPosition(Vector3 force, Vector3 position, Force_Mode mode = Force_Mode::Force) const;

	private:
		void Initialize(const std::shared_ptr<Collider>& col);
		void Create();
		void Resize() const;
		void Remove();
		void Get_BtTransform(btTransform& t) const;
		void Set_BtTransform(const btTransform& t) const;
		void Set_Debug_Draw(bool value) const;
		bool Get_Debug_Drawed() const;

		std::unique_ptr<btRigidBody> rigidbody;
		std::unique_ptr<btDefaultMotionState> motion_state;
		std::weak_ptr<Collider> collider;

		float mass = 1.0f;
		float send_mass = 1.0f;
		bool is_dynamic = true;
		bool is_kinematic = false;
		Vector3 gravity = { 0.0f, -9.8f, 0.0f };
		Vector3 linear_factor = { 1.0f, 1.0f, 1.0f };
		Vector3 angular_factor = { 1.0f, 1.0f, 1.0f };

		friend class Collider;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(mass, is_dynamic, is_kinematic, gravity, linear_factor, angular_factor);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::RigidBody, 1)