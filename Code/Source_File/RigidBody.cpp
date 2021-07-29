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

void RigidBody::Initialize(shared_ptr<Collider> col)
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
	shared_ptr<Collider> col = collider.lock();

	Quaternion q = col->transform->Get_Rotation();
	Vector3 p = col->transform->Get_Position();
	btTransform transform(btQuaternion(q.x, q.y, q.z, q.w), btVector3(p.x, p.y, p.z));

	is_dynamic = (send_mass != 0.0f);

	//Static判定
	btVector3 localInertia(0, 0, 0);
	if (is_dynamic)
	{
		col->shape->calculateLocalInertia(send_mass, localInertia);
	}

	motion_state = make_unique<btDefaultMotionState>(transform);

	//剛体作成
	btRigidBody::btRigidBodyConstructionInfo rb_info(send_mass, motion_state.get(), col->shape.get(), localInertia);
	rigidbody = make_unique<btRigidBody>(btRigidBody(rb_info));

	if (is_kinematic)
	{
		rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidbody->setActivationState(DISABLE_DEACTIVATION);
	}

	Engine::bulletphysics_manager->Add_RigidBody(collider, rigidbody, col->gameobject->layer, -1);
	rigidbody->setLinearFactor(btVector3(linear_factor.x, linear_factor.y, linear_factor.z));
	rigidbody->setAngularFactor(btVector3(angular_factor.x, angular_factor.y, angular_factor.z));
	rigidbody->setGravity(btVector3(gravity.x, gravity.y, gravity.z));

	//デバッグ非表示
	Set_Debug_Draw(false);
}

void RigidBody::Resize()
{
	Engine::bulletphysics_manager->Resize_RigidBody(rigidbody);
}

void RigidBody::Remove()
{
	Engine::bulletphysics_manager->Remove_RigidBody(rigidbody);
}

float RigidBody::Get_Mass() const
{
	return send_mass;
}

void RigidBody::Set_Mass(float set_mass)
{
	if (is_dynamic)
	{
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

void RigidBody::Set_Kinematic(bool value)
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

void RigidBody::Set_Dynamic(bool value)
{
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

void RigidBody::Get_btTransform(btTransform& t)
{
	if (rigidbody->isStaticOrKinematicObject())
	{
		t = rigidbody->getWorldTransform();
	}
	else
	{
		btMotionState* motion = rigidbody->getMotionState();
		motion->getWorldTransform(t);
	}
}

void RigidBody::Set_btTransform(btTransform& t)
{
	rigidbody->activate(true);
	rigidbody->setWorldTransform(t);
}

void RigidBody::Set_Freeze_Position(bool x_axis, bool y_axis, bool z_axis)
{
	rigidbody->activate(true);
	linear_factor = { static_cast<float>(!x_axis), static_cast<float>(!y_axis), static_cast<float>(!z_axis) };
	rigidbody->setLinearFactor(btVector3(linear_factor.x, linear_factor.y, linear_factor.z));
}

void RigidBody::Set_Freeze_Rotation(bool x_axis, bool y_axis, bool z_axis)
{
	rigidbody->activate(true);
	angular_factor = { static_cast<float>(!x_axis), static_cast<float>(!y_axis), static_cast<float>(!z_axis) };
	rigidbody->setAngularFactor(btVector3(angular_factor.x, angular_factor.y, angular_factor.z));
}

void RigidBody::Set_Angular_Velocity(Vector3 velocity)
{
	rigidbody->activate(true);
	rigidbody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

Vector3 RigidBody::Get_Angular_Velocity() const
{
	btVector3 bt_v = rigidbody->getAngularVelocity();
	Vector3 v = { bt_v.x(), bt_v.y(), bt_v.z() };
	return v;
}

void RigidBody::Set_Velocity(Vector3 velocity)
{
	rigidbody->activate(true);
	rigidbody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

Vector3 RigidBody::Get_Velocity() const
{
	btVector3 bt_v = rigidbody->getLinearVelocity();
	Vector3 v = { bt_v.x(),bt_v.y(), bt_v.z() };
	return v;
}

void RigidBody::Use_Gravity(bool value)
{
	if (value) gravity = { 0.0f, -9.8f, 0.0f };
	else gravity = { 0.0f, 0.0f, 0.0f };

	rigidbody->activate(true);
	rigidbody->setGravity(btVector3(gravity.x, gravity.y, gravity.z));

	btVector3 bt_v = rigidbody->getLinearVelocity();
	rigidbody->setLinearVelocity(btVector3(bt_v.x(), 0.0f, bt_v.z()));
}

void RigidBody::Add_Force(Vector3 force, Force_Mode mode)
{
	rigidbody->activate(true);
	btVector3 f = { force.x, force.y, force.z };
	switch (mode)
	{
		case BeastEngine::Force_Mode::Force:
			rigidbody->applyCentralForce(f);
			break;
		case BeastEngine::Force_Mode::Impulse:
			rigidbody->applyCentralImpulse(f);
			break;
		default:
			break;
	}
}

void RigidBody::Add_Force_AtPosition(Vector3 force, Vector3 position, Force_Mode mode)
{
	rigidbody->activate(true);
	btVector3 f = { force.x, force.y, force.z };
	btVector3 p = { position.x, position.y, position.z };
	switch (mode)
	{
		case BeastEngine::Force_Mode::Force:
			rigidbody->applyForce(f, p);
			break;
		case BeastEngine::Force_Mode::Impulse:
			rigidbody->applyImpulse(f, p);
			break;
		default:
			break;
	}
}

void RigidBody::Set_Debug_Draw(bool value)
{
	if(value) rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() & ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
	else rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
}