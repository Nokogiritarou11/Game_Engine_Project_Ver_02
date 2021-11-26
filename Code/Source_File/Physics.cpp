#include "Physics.h"
#include "Engine.h"
#include "BulletPhysics_Manager.h"

using namespace std;
using namespace BeastEngine;

bool Physics::Raycast(const Vector3& from, const Vector3& to)
{
	const btVector3 bt_from(from.x, from.y, from.z);
	const btVector3 bt_to(to.x, to.y, to.z);
	btCollisionWorld::ClosestRayResultCallback res(bt_from, bt_to);

	//���C�L���X�g�B�R�[���o�b�N�ɏՓˏ�񂪓����Ă���
	Engine::bulletphysics_manager->world->rayTest(bt_from, bt_to, res);

	return res.hasHit();
}