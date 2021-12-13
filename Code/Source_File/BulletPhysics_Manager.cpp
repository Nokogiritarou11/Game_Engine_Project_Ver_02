#include "BulletPhysics_Manager.h"
#include "Debug_Draw_Manager.h"
#include "Collider.h"
#include "Time_Engine.h"
#include "Engine.h"
#include "Scene_Manager.h"
#include "Project_Settings.h"

using namespace std;
using namespace BeastEngine;

BulletPhysics_Manager::BulletPhysics_Manager()
{
	Initialize();
}

void BulletPhysics_Manager::Initialize()
{
	//各メンバの作成
	collision_config = make_unique<btDefaultCollisionConfiguration>();
	dispatcher = make_unique<btCollisionDispatcher>(collision_config.get());
	broadphase = make_unique<btDbvtBroadphase>();
	solver = make_unique<btSequentialImpulseConstraintSolver>();

	world = make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), collision_config.get());
	world->setGravity(btVector3(0, -9.8f, 0));

	ghost_pair_call = make_unique<btGhostPairCallback>();
	world->getPairCache()->setInternalGhostPairCallback(ghost_pair_call.get());
}

void BulletPhysics_Manager::Reset()
{
	//各オブジェクトを末尾から削除
	for (int i = world->getNumConstraints() - 1; i >= 0; --i)
	{
		btTypedConstraint* ct = world->getConstraint(i);
		world->removeConstraint(ct);
	}

	for (int i = world->getNumCollisionObjects() - 1; i >= 0; --i)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	collider_list.clear();
}

void BulletPhysics_Manager::Exit()
{
	Reset();

	world.reset();
	solver.reset();
	broadphase.reset();
	dispatcher.reset();
	collision_config.reset();
}

void BulletPhysics_Manager::Update()
{
	//シーンでの姿勢をBullet内に反映
	for (auto& col : collider_list)
	{
		col.second.lock()->Update_Transform();
	}

	if (Engine::scene_manager->run)
	{
		typedef pair<shared_ptr<Collider>, Collision> hit_pair;
		vector<hit_pair> hit_list;
		//Bullet内の時間を進める
		constexpr float fixed_step = 1.0f / 360.0f;
		//world->stepSimulation(Time::delta_time, 2, Time::delta_time * 0.5f);
		world->stepSimulation(Time::delta_time,6, fixed_step);

		//衝突判定の数だけ回す
		const int num = dispatcher->getNumManifolds();
		for (int i = 0; i < num; ++i)
		{
			const btPersistentManifold* contact_manifold = dispatcher->getManifoldByIndexInternal(i);
			// 衝突物体
			const btCollisionObject* obj_a = contact_manifold->getBody0();
			const btCollisionObject* obj_b = contact_manifold->getBody1();

			vector<Vector3> contacts_a, contacts_b;
			const int num_contacts = contact_manifold->getNumContacts();
			for (int j = 0; j < num_contacts; ++j)
			{
				if (const btManifoldPoint& pt = contact_manifold->getContactPoint(j); pt.getDistance() < 0.f)
				{
					// 衝突した
					btVector3 pos_a = pt.getPositionWorldOnA();
					contacts_a.emplace_back(Vector3(pos_a.x(), pos_a.y(), pos_a.z()));
					btVector3 pos_b = pt.getPositionWorldOnB();
					contacts_b.emplace_back(Vector3(pos_b.x(), pos_b.y(), pos_b.z()));
				}
			}
			//衝突していた場合はコールバックリストに追加
			if (!contacts_a.empty())
			{
				const shared_ptr<Collider>& col_a = collider_list[obj_a].lock();
				const shared_ptr<Collider>& col_b = collider_list[obj_b].lock();
				Collision collision_a = { col_b, col_b->gameobject, col_b->transform, contacts_b };
				Collision collision_b = { col_a, col_a->gameobject, col_a->transform, contacts_a };
				hit_list.emplace_back(hit_pair(col_a, collision_a));
				hit_list.emplace_back(hit_pair(col_b, collision_b));
			}
		}

		//Bullet内での変更をシーンに反映
		for (auto& col : collider_list)
		{
			col.second.lock()->Update_Simulation();
		}

		//衝突コールバックを呼ぶ
		if (!hit_list.empty())
		{
			for (auto& hit : hit_list)
			{
				hit.first->Call_Hit(hit.second);
			}
		}
	}
}

void BulletPhysics_Manager::Add_RigidBody(const weak_ptr<Collider> col, const unique_ptr<btRigidBody>& rigidbody, const int layer)
{
	if (const auto it = collider_list.find(rigidbody.get()); it == collider_list.end())
	{
		world->addRigidBody(rigidbody.get(), (1 << layer), Engine::scene_manager->settings->layer_mask[layer]);
		collider_list[rigidbody.get()] = col;
	}
}

void BulletPhysics_Manager::Resize_RigidBody(const unique_ptr<btRigidBody>& rigidbody)
{
	if (const auto it = collider_list.find(rigidbody.get()); it != collider_list.end())
	{
		world->updateSingleAabb(rigidbody.get());
	}
}

void BulletPhysics_Manager::Remove_RigidBody(const unique_ptr<btRigidBody>& rigidbody)
{
	if (const auto it = collider_list.find(rigidbody.get()); it != collider_list.end())
	{
		world->removeRigidBody(rigidbody.get());
		collider_list.erase(it);
	}
}

void BulletPhysics_Manager::Add_Ghost(const weak_ptr<Collider> col, const unique_ptr<btGhostObject>& ghost, const int layer)
{
	if (const auto it = collider_list.find(ghost.get()); it == collider_list.end())
	{
		world->addCollisionObject(ghost.get(), (1 << layer), Engine::scene_manager->settings->layer_mask[layer]);
		collider_list[ghost.get()] = col;
	}
}

void BulletPhysics_Manager::Resize_Ghost(const unique_ptr<btGhostObject>& ghost)
{
	if (const auto it = collider_list.find(ghost.get()); it != collider_list.end())
	{
		world->updateSingleAabb(ghost.get());
	}
}

void BulletPhysics_Manager::Remove_Ghost(const unique_ptr<btGhostObject>& ghost)
{
	if (const auto it = collider_list.find(ghost.get()); it != collider_list.end())
	{
		world->removeCollisionObject(ghost.get());
		collider_list.erase(it);
	}
}

void BulletPhysics_Manager::Set_Debug_Drawer() const
{
	if (Engine::debug_draw_manager)
	{
		Engine::debug_draw_manager->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		world->setDebugDrawer(Engine::debug_draw_manager.get());
	}
}

void BulletPhysics_Manager::Render_Debug() const
{
	world->debugDrawWorld();
}
