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
	Engine::bulletphysics_manager->Remove_RigidBody(rigidbody);
}

void RigidBody::Initialize(shared_ptr<Collider> col, unique_ptr<btCollisionShape>& shape, shared_ptr<Transform> trans)
{
	collider = col;

	//épê®
	Quaternion q = trans->Get_Rotation();
	Vector3 p = trans->Get_Position();
	btTransform transform(btQuaternion(q.x, q.y, q.z, q.w), btVector3(p.x, p.y, p.z));

	bool is_dynamic = (mass != 0.0f);

	//StaticîªíË
	btVector3 localInertia(0, 0, 0);
	if (is_dynamic)
	{
		shape->calculateLocalInertia(mass, localInertia);
	}

	motion_state = make_unique<btDefaultMotionState>(transform);

	//çÑëÃçÏê¨
	btRigidBody::btRigidBodyConstructionInfo rb_info(mass, motion_state.get(), shape.get(), localInertia);
	rigidbody = make_unique<btRigidBody>(btRigidBody(rb_info));
	Engine::bulletphysics_manager->Add_RigidBody(collider, rigidbody, col->gameobject->layer, 0);
}

void RigidBody::Create_RigidBody()
{
	Engine::bulletphysics_manager->Remove_RigidBody(rigidbody);

	//épê®
	shared_ptr<Collider> col = collider.lock();
	Quaternion q = col->transform->Get_Rotation();
	Vector3 p = col->transform->Get_Position();
	btTransform transform(btQuaternion(q.x, q.y, q.z, q.w), btVector3(p.x, p.y, p.z));

	bool is_dynamic = (mass != 0.0f);

	//StaticîªíË
	btVector3 localInertia(0, 0, 0);
	if (is_dynamic)
	{
		col->shape->calculateLocalInertia(mass, localInertia);
	}

	motion_state = make_unique<btDefaultMotionState>(transform);

	//çÑëÃçÏê¨
	btRigidBody::btRigidBodyConstructionInfo rb_info(mass, motion_state.get(), col->shape.get(), localInertia);
	rigidbody = make_unique<btRigidBody>(btRigidBody(rb_info));
	Engine::bulletphysics_manager->Add_RigidBody(collider, rigidbody, col->gameobject->layer, 0);
}

void RigidBody::Set_Mass(float set_mass)
{
	if (mass != set_mass)
	{
		if (set_mass == 0)
		{
			set_mass = 0.000001f;
		}
		mass = set_mass;
		Create_RigidBody();
	}
}

void RigidBody::Set_Kinematic(bool value)
{
	if (mass != 0)
	{
		if (!value)
		{
			mass = 0.0f;
			Create_RigidBody();
		}
	}
	else
	{
		if (value)
		{
			mass = 1.0f;
			Create_RigidBody();
		}
	}
}

void RigidBody::Set_Freeze_Position(bool x_axis, bool y_axis, bool z_axis)
{
	linear_factor = { static_cast<float>(x_axis), static_cast<float>(y_axis), static_cast<float>(z_axis) };
	rigidbody->setLinearFactor(btVector3(linear_factor.x, linear_factor.y, linear_factor.z));
}

void RigidBody::Set_Freeze_Rotation(bool x_axis, bool y_axis, bool z_axis)
{
	angular_factor = { static_cast<float>(x_axis), static_cast<float>(y_axis), static_cast<float>(z_axis) };
	rigidbody->setAngularFactor(btVector3(angular_factor.x, angular_factor.y, angular_factor.z));
}

void RigidBody::Set_Angular_Velocity(Vector3 velocity)
{
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

	rigidbody->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

void RigidBody::Add_Force(Vector3 force, Force_Mode mode)
{
	btVector3 f = { force.x, force.y, force.z };
	rigidbody->activate(true);
	switch (mode)
	{
		case BeastEngine::Force_Mode::Force:
			rigidbody->applyCentralForce(f);
			break;
			/*case BeastEngine::Force_Mode::Acceleration:
				break;*/
		case BeastEngine::Force_Mode::Impulse:
			rigidbody->applyCentralImpulse(f);
			break;
			/*case BeastEngine::Force_Mode::VelocityChange:
				break;*/
		default:
			break;
	}
}

void RigidBody::Add_Force_AtPosition(Vector3 force, Vector3 position, Force_Mode mode)
{
	btVector3 f = { force.x, force.y, force.z };
	btVector3 p = { position.x, position.y, position.z };
	rigidbody->activate(true);
	switch (mode)
	{
		case BeastEngine::Force_Mode::Force:
			rigidbody->applyForce(f, p);
			break;
			/*case BeastEngine::Force_Mode::Acceleration:
				break;*/
		case BeastEngine::Force_Mode::Impulse:
			rigidbody->applyImpulse(f, p);
			break;
			/*case BeastEngine::Force_Mode::VelocityChange:
				break;*/
		default:
			break;
	}
}

void RigidBody::Draw_ImGui()
{
}