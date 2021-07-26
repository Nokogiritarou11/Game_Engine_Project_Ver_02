#include "Collider.h"
#include "GameObject.h"
#include "MonoBehaviour.h"
#include "Engine.h"
#include "BulletPhysics_Manager.h"
#include "Asset_Manager.h"

using namespace BeastEngine;
using namespace std;

Collider::~Collider()
{
	if (ghost)
	{
		Engine::bulletphysics_manager->Remove_Ghost(ghost);
	}
}

void Collider::Set_Enabled(bool value)
{
	if (value != enabled_old)
	{
		enabled = value;
		enabled_old = value;
		Set_Active(value);
	}
}

bool Collider::Get_Enabled()
{
	return enabled;
}

void Collider::Set_Active(bool value)
{
	if (value)
	{
		if (gameobject->Get_Active_In_Hierarchy())
		{
			if (Get_Enabled())
			{
				if (!is_called)
				{
					Initialize_MonoBehaviour();
					is_called = true;
				}
				disabled = false;
			}
			else
			{
				disabled = true;
			}
		}
		else
		{
			disabled = true;
		}
	}
	else
	{
		disabled = true;
	}

	if (disabled != disabled_old)
	{
		disabled = disabled_old;
		if (disabled)
		{
			if (is_trigger)
			{
				Engine::bulletphysics_manager->Remove_Ghost(ghost);
			}
			else
			{
				Engine::bulletphysics_manager->Remove_RigidBody(rigidbody->rigidbody);
				rigidbody->rigidbody.reset();
			}
			shape.reset();
		}
		else
		{
			Create_Shape();
			Create_Collider();
		}
	}
}

void Collider::Create_Ghost()
{
	ghost = make_unique<btGhostObject>();

	// 形状設定
	ghost->setCollisionShape(shape.get());
	// 他のオブジェクトへの反応なし
	// CF_NO_CONTACT_RESPONSEを指定しないと剛体と衝突する
	ghost->setCollisionFlags(ghost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	// 位置設定
	Vector3 pos = transform->Get_Position();
	Quaternion rot = transform->Get_Rotation();
	btTransform t(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(pos.x, pos.y, pos.z));
	ghost->setWorldTransform(t);
}

void Collider::Create_Collider()
{
	if (is_trigger)
	{
		rigidbody.reset();

		if (!ghost)
		{
			Create_Ghost();
		}
		Engine::bulletphysics_manager->Add_Ghost(static_pointer_cast<Collider>(shared_from_this()), ghost, gameobject->layer, 0);
	}
	else
	{
		if (ghost)
		{
			Engine::bulletphysics_manager->Remove_Ghost(ghost);
			ghost.reset();
		}
		if (!rigidbody)
		{
			rigidbody = make_unique<RigidBody>();
		}
		rigidbody->Initialize(static_pointer_cast<Collider>(shared_from_this()), shape, transform);
	}
}

void Collider::Set_IsTrigger(bool value)
{
	if (is_trigger != value)
	{
		is_trigger = value;
		Create_Collider();
	}
}

void Collider::Initialize(shared_ptr<GameObject> obj)
{
	gameobject = obj;
	Engine::asset_manager->Registration_Asset(shared_from_this());
	transform = obj->transform;

	Create_Shape();
	Create_Collider();

	Set_Active(gameobject->Get_Active_In_Hierarchy());
}


void Collider::Initialize_MonoBehaviour()
{
	for (auto& com : gameobject->component_list)
	{
		shared_ptr<MonoBehaviour> buff = dynamic_pointer_cast<MonoBehaviour>(com);
		if (buff != nullptr)
		{
			send_list.emplace_back(buff);
		}
	}
}

void Collider::Call_Hit(Collision& collision)
{
	string& id = collision.collider->Get_Instance_ID();
	auto it = hit_list.find(id);
	if (it == hit_list.end())
	{
		hit_list[id] = collision.collider;
		if (collision.collider->is_trigger)
		{
			Call_OnTrigger_Enter(collision);
		}
		else
		{
			Call_OnCollision_Enter(collision);
		}
	}
	else
	{
		if (collision.collider->is_trigger)
		{
			Call_OnTrigger_Stay(collision);
		}
		else
		{
			Call_OnCollision_Stay(collision);
		}
	}
}

void Collider::Call_Update()
{

}

void Collider::Call_OnTrigger_Enter(Collision& collision)
{
	shared_ptr<MonoBehaviour> mono;
	for (auto& m : send_list)
	{
		if (!m.expired())
		{
			mono = m.lock();
			mono->OnTrigger_Enter(collision);
		}
	}
}
void Collider::Call_OnTrigger_Stay(Collision& collision)
{
	shared_ptr<MonoBehaviour> mono;
	for (auto& m : send_list)
	{
		if (!m.expired())
		{
			mono = m.lock();
			mono->OnTrigger_Stay(collision);
		}
	}
}
void Collider::Call_OnTrigger_Exit(Collision& collision)
{
	shared_ptr<MonoBehaviour> mono;
	for (auto& m : send_list)
	{
		if (!m.expired())
		{
			mono = m.lock();
			mono->OnTrigger_Exit(collision);
		}
	}
}

void Collider::Call_OnCollision_Enter(Collision& collision)
{
	shared_ptr<MonoBehaviour> mono;
	for (auto& m : send_list)
	{
		if (!m.expired())
		{
			mono = m.lock();
			mono->OnCollision_Enter(collision);
		}
	}
}
void Collider::Call_OnCollision_Stay(Collision& collision)
{
	shared_ptr<MonoBehaviour> mono;
	for (auto& m : send_list)
	{
		if (!m.expired())
		{
			mono = m.lock();
			mono->OnCollision_Stay(collision);
		}
	}
}
void Collider::Call_OnCollision_Exit(Collision& collision)
{
	shared_ptr<MonoBehaviour> mono;
	for (auto& m : send_list)
	{
		if (!m.expired())
		{
			mono = m.lock();
			mono->OnCollision_Exit(collision);
		}
	}
}