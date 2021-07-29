#include "BulletPhysics_Manager.h"
#include "Debug_Draw_Manager.h"
#include "Collider.h"
#include "Time_Engine.h"
#include "Engine.h"
#include "Scene_Manager.h"

using namespace std;
using namespace BeastEngine;

BulletPhysics_Manager::BulletPhysics_Manager()
{
	Initialize();
}

void BulletPhysics_Manager::Initialize()
{
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
	for (auto& col : collider_list)
	{
		col.second.lock()->Update_Transform();
	}

	if (Engine::scene_manager->run)
	{
		world->stepSimulation(Time::delta_time);

		int num = dispatcher->getNumManifolds();
		for (int i = 0; i < num; ++i)
		{
			const btPersistentManifold* contactManifold = dispatcher->getManifoldByIndexInternal(i);
			// Õ“Ë•¨‘Ì
			const btCollisionObject* obA = contactManifold->getBody0();
			const btCollisionObject* obB = contactManifold->getBody1();

			vector<Vector3> contacts_A, contacts_B;
			int num_contacts = contactManifold->getNumContacts();
			for (int j = 0; j < num_contacts; ++j)
			{
				// Õ“Ë“_‚Ìî•ñ‚ðŽæ“¾
				const btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if (pt.getDistance() < 0.f)
				{
					// ŽÀÛ‚ÉÕ“Ë‚µ‚½
					//auto normal = pt.m_normalWorldOnB;
					//auto impulse = pt.getAppliedImpulse();

					btVector3 pos_A = pt.getPositionWorldOnA();
					contacts_A.emplace_back(Vector3(pos_A.x(), pos_A.y(), pos_A.z()));
					btVector3 pos_B = pt.getPositionWorldOnB();
					contacts_B.emplace_back(Vector3(pos_B.x(), pos_B.y(), pos_B.z()));
				}
			}
			if (!contacts_A.empty())
			{
				shared_ptr<Collider> col_A = collider_list[obA].lock(), col_B = collider_list[obB].lock();
				Collision collision_A = { col_B, col_B->gameobject, col_B->transform, contacts_B };
				col_A->Call_Hit(collision_A);
				Collision collision_B = { col_A, col_A->gameobject, col_A->transform, contacts_A };
				col_B->Call_Hit(collision_B);
			}
		}

		for (auto& col : collider_list)
		{
			col.second.lock()->Update_Simulation();
		}
	}
}

void BulletPhysics_Manager::Add_RigidBody(weak_ptr<Collider> col, unique_ptr<btRigidBody>& rigidbody, int group, int mask)
{
	auto it = collider_list.find(rigidbody.get());
	if (it == collider_list.end())
	{
		world->addRigidBody(rigidbody.get());
		collider_list[rigidbody.get()] = col;
	}
}

void BulletPhysics_Manager::Resize_RigidBody(unique_ptr<btRigidBody>& rigidbody)
{
	auto it = collider_list.find(rigidbody.get());
	if (it != collider_list.end())
	{
		world->updateSingleAabb(rigidbody.get());
	}
}

void BulletPhysics_Manager::Remove_RigidBody(unique_ptr<btRigidBody>& rigidbody)
{
	auto it = collider_list.find(rigidbody.get());
	if (it != collider_list.end())
	{
		world->removeRigidBody(rigidbody.get());
		collider_list.erase(it);
	}
}

void BulletPhysics_Manager::Add_Ghost(weak_ptr<Collider> col, unique_ptr<btGhostObject>& ghost, int group, int mask)
{
	auto it = collider_list.find(ghost.get());
	if (it == collider_list.end())
	{
		world->addCollisionObject(ghost.get());
		collider_list[ghost.get()] = col;
	}
}

void BulletPhysics_Manager::Resize_Ghost(unique_ptr<btGhostObject>& ghost)
{
	auto it = collider_list.find(ghost.get());
	if (it != collider_list.end())
	{
		world->updateSingleAabb(ghost.get());
	}
}

void BulletPhysics_Manager::Remove_Ghost(unique_ptr<btGhostObject>& ghost)
{
	auto it = collider_list.find(ghost.get());
	if (it != collider_list.end())
	{
		world->removeCollisionObject(ghost.get());
		collider_list.erase(it);
	}
}

void BulletPhysics_Manager::Set_Debug_Drawer()
{
	if (Engine::debug_draw_manager)
	{
		Engine::debug_draw_manager->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		world->setDebugDrawer(Engine::debug_draw_manager.get());
	}
}

void BulletPhysics_Manager::Render_Debug()
{
	world->debugDrawWorld();
}