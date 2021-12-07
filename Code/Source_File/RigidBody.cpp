#include "RigidBody.h"
#include "Engine.h"
#include "BulletPhysics_Manager.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"
using namespace std;
using namespace BeastEngine;

RigidBody::~RigidBody()
{
	Remove();
}

void RigidBody::Initialize(const shared_ptr<Collider>& col)
{
	collider = col;
	if (is_dynamic)
	{
		send_mass = mass;
	}
	else
	{
		send_mass = 0.0f;
	}
	Create();
}

void RigidBody::Create()
{
	if (rigidbody)
	{
		Engine::bulletphysics_manager->Remove_RigidBody(rigidbody);
	}
	//姿勢
	const shared_ptr<Collider>& col = collider.lock();

	const Vector3 pos = col->transform->Get_Position() + (col->transform->Get_Left() * col->center.x) + (col->transform->Get_Up() * col->center.y) + (col->transform->Get_Forward() * col->center.z);
	const Quaternion rot = col->transform->Get_Rotation();
	btTransform t(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(pos.x, pos.y, pos.z));

	is_dynamic = (send_mass != 0.0f);

	//Static判定
	btVector3 local_inertia(0, 0, 0);
	if (is_dynamic)
	{
		col->shape->calculateLocalInertia(send_mass, local_inertia);
	}

	motion_state = make_unique<btDefaultMotionState>(t);

	//剛体作成
	const btRigidBody::btRigidBodyConstructionInfo rb_info(send_mass, motion_state.get(), col->shape.get(), local_inertia);
	rigidbody = make_unique<btRigidBody>(btRigidBody(rb_info));

	if (is_kinematic)
	{
		rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidbody->setActivationState(DISABLE_DEACTIVATION);
	}

	//パラメータ設定
	Engine::bulletphysics_manager->Add_RigidBody(collider, rigidbody, col->gameobject->layer);
	rigidbody->setLinearFactor(btVector3(linear_factor.x, linear_factor.y, linear_factor.z));
	rigidbody->setAngularFactor(btVector3(angular_factor.x, angular_factor.y, angular_factor.z));
	rigidbody->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
	rigidbody->setFriction(0);
	rigidbody->setRestitution(0);
}

void RigidBody::Resize() const
{
	Engine::bulletphysics_manager->Resize_RigidBody(rigidbody);
}

void RigidBody::Remove()
{
	if (rigidbody)
	{
		Engine::bulletphysics_manager->Remove_RigidBody(rigidbody);
		rigidbody.reset();
	}
}

float RigidBody::Get_Mass() const
{
	return send_mass;
}

void RigidBody::Set_Mass(float set_mass)
{
	if (is_dynamic)
	{
		//重いので変更時のみ
		if (mass != set_mass)
		{
			if (set_mass <= 0)
			{
				set_mass = 0.000001f;
			}
			mass = set_mass;
			send_mass = mass;
			Create();
		}
	}
}

void RigidBody::Set_Kinematic(const bool value)
{
	if (is_dynamic)
	{
		if (value)
		{
			is_dynamic = false;
			is_kinematic = value;
			send_mass = 0.0f;
			Create();
		}
	}
	else
	{
		if (is_kinematic != value)
		{
			is_kinematic = value;
			if (is_kinematic)
			{
				rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
				rigidbody->setActivationState(DISABLE_DEACTIVATION);
			}
			else
			{
				rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
				rigidbody->setActivationState(ACTIVE_TAG);
			}
		}
	}
}

void RigidBody::Set_Dynamic(const bool value)
{
	//重いので変更時のみ
	if (is_dynamic != value)
	{
		is_dynamic = value;
		if (is_dynamic)
		{
			send_mass = mass;
			is_kinematic = false;
		}
		else
		{
			send_mass = 0.0f;
		}
		Create();
	}
}

void RigidBody::Get_BtTransform(btTransform& t) const
{
	t = rigidbody->getWorldTransform();
	//rigidbody->getMotionState()->getWorldTransform(t);
	/*
	if (rigidbody->isStaticOrKinematicObject())
	{
		t = rigidbody->getWorldTransform();
	}
	else
	{
		motion_state->getWorldTransform(t);
	}
	*/
}

void RigidBody::Set_BtTransform(const btTransform& t) const
{
	rigidbody->activate(true);
	rigidbody->setWorldTransform(t);
	motion_state->setWorldTransform(t);
}

void RigidBody::Set_Freeze_Position(const bool x_axis, const bool y_axis, const bool z_axis)
{
	rigidbody->activate(true);
	linear_factor = Vector3(!x_axis, !y_axis, !z_axis);
	rigidbody->setLinearFactor(btVector3(linear_factor.x, linear_factor.y, linear_factor.z));
}

void RigidBody::Set_Freeze_Rotation(bool x_axis, bool y_axis, bool z_axis)
{
	rigidbody->activate(true);
	angular_factor = Vector3(!x_axis, !y_axis, !z_axis);
	rigidbody->setAngularFactor(btVector3(angular_factor.x, angular_factor.y, angular_factor.z));
}

void RigidBody::Set_Angular_Velocity(const Vector3 velocity) const
{
	rigidbody->activate(true);
	rigidbody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

Vector3 RigidBody::Get_Angular_Velocity() const
{
	const btVector3 bt_v = rigidbody->getAngularVelocity();
	const Vector3 v = { bt_v.x(), bt_v.y(), bt_v.z() };
	return v;
}

void RigidBody::Set_Velocity(const Vector3 velocity) const
{
	rigidbody->activate(true);
	rigidbody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

Vector3 RigidBody::Get_Velocity() const
{
	const btVector3 bt_v = rigidbody->getLinearVelocity();
	const Vector3 v = { bt_v.x(),bt_v.y(), bt_v.z() };
	return v;
}

void RigidBody::Use_Gravity(const bool value)
{
	if (value)
	{
		if (gravity == Vector3(0.0f, -9.8f, 0.0f)) return;
		gravity = Vector3(0.0f, -9.8f, 0.0f);
	}
	else
	{
		if (gravity == Vector3(0.0f, 0.0f, 0.0f)) return;
		gravity = Vector3(0.0f, 0.0f, 0.0f);
	}
	rigidbody->activate(true);
	rigidbody->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

void RigidBody::Add_Force(const Vector3 force, const Force_Mode mode) const
{
	rigidbody->activate(true);
	const btVector3 f = { force.x, force.y, force.z };
	switch (mode)
	{
		case Force_Mode::Force:
			rigidbody->applyCentralForce(f);
			break;
		case Force_Mode::Impulse:
			rigidbody->applyCentralImpulse(f);
			break;
		default:
			break;
	}
}

void RigidBody::Add_Force_AtPosition(const Vector3 force, const Vector3 position, const Force_Mode mode) const
{
	rigidbody->activate(true);
	const btVector3 f = { force.x, force.y, force.z };
	const btVector3 p = { position.x, position.y, position.z };
	switch (mode)
	{
		case Force_Mode::Force:
			rigidbody->applyForce(f, p);
			break;
		case Force_Mode::Impulse:
			rigidbody->applyImpulse(f, p);
			break;
		default:
			break;
	}
}

void RigidBody::Set_Debug_Draw(const bool value) const
{
	if (rigidbody)
	{
		if (value) rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() & ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
		else rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
	}
}

bool RigidBody::Get_Debug_Drawed() const
{
	return !(rigidbody->getCollisionFlags() & btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
}
