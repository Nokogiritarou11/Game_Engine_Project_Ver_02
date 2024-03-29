#include "GhostObject.h"
#include "Engine.h"
#include "BulletPhysics_Manager.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"

using namespace std;
using namespace BeastEngine;

GhostObject::~GhostObject()
{
	Remove();
}

void GhostObject::Initialize(shared_ptr<Collider> col)
{
	collider = col;
	Create();
}

void GhostObject::Create()
{
	ghost = make_unique<btGhostObject>();
	const shared_ptr<Collider> col = collider.lock();

	// 形状設定
	ghost->setCollisionShape(col->shape.get());
	// 他のオブジェクトへの反応なし
	// CF_NO_CONTACT_RESPONSEを指定しないと剛体と衝突する
	ghost->setCollisionFlags(ghost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	// 位置設定
	const Vector3 pos = col->transform->Get_Position() + (col->transform->Get_Left() * col->center.x) + (col->transform->Get_Up() * col->center.y) + (col->transform->Get_Forward() * col->center.z);
	const Quaternion rot = col->transform->Get_Rotation();
	const btTransform t(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(pos.x, pos.y, pos.z));
	ghost->setWorldTransform(t);
	Engine::bulletphysics_manager->Add_Ghost(col, ghost, col->gameobject->layer);
}

void GhostObject::Resize() const
{
	Engine::bulletphysics_manager->Resize_Ghost(ghost);
}

void GhostObject::Remove()
{
	if (ghost)
	{
		Engine::bulletphysics_manager->Remove_Ghost(ghost);
		ghost.reset();
	}
}

void GhostObject::Get_BtTransform(btTransform& t) const
{
	t = ghost->getWorldTransform();
}

void GhostObject::Set_BtTransform(const btTransform& t) const
{
	ghost->setWorldTransform(t);
}

void GhostObject::Set_Debug_Draw(bool value) const
{
	if (ghost)
	{
		if (value) ghost->setCollisionFlags(ghost->getCollisionFlags() & ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
		else ghost->setCollisionFlags(ghost->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
	}
}

bool GhostObject::Get_Debug_Drawed() const
{
	return !(ghost->getCollisionFlags() & btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
}
