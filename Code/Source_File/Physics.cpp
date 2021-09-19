#include "Physics.h"
#include "Engine.h"
#include "BulletPhysics_Manager.h"

using namespace std;
using namespace BeastEngine;

bool Physics::Raycast(const Vector3& from, const Vector3& to)
{
	btVector3 btFrom(from.x, from.y, from.z);
	btVector3 btTo(to.x, to.y, to.z);
	btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

	Engine::bulletphysics_manager->world->rayTest(btFrom, btTo, res);

	return res.hasHit();
}