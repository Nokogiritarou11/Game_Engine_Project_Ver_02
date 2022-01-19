#include "Physics.h"
#include "Engine.h"
#include "BulletPhysics_Manager.h"

using namespace std;
using namespace BeastEngine;

bool Physics::Raycast(const Vector3& from, const Vector3& to, Vector3& hit_pos_res, const int layer_mask)
{
	const btVector3 bt_from(from.x, from.y, from.z);
	const btVector3 bt_to(to.x, to.y, to.z);
	btCollisionWorld::ClosestRayResultCallback res(bt_from, bt_to);
	res.m_collisionFilterMask = layer_mask;

	//レイキャスト。コールバックに衝突情報が入ってくる
	Engine::bulletphysics_manager->world->rayTest(bt_from, bt_to, res);

	if (res.hasHit())
	{
		hit_pos_res = Vector3(res.m_hitPointWorld.x(), res.m_hitPointWorld.y(), res.m_hitPointWorld.z());
		return true;
	}

	return false;
}