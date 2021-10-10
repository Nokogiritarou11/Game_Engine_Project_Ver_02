#pragma once
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include <memory>
#include <unordered_map>

namespace BeastEngine
{
	class Collider;

	class BulletPhysics_Manager
	{
	public:
		BulletPhysics_Manager();
		void Initialize();
		void Reset();
		void Exit();
		void Update();

		void Add_RigidBody(std::weak_ptr<Collider> col, const std::unique_ptr<btRigidBody>& rigidbody, int layer);
		void Resize_RigidBody(const std::unique_ptr<btRigidBody>& rigidbody);
		void Remove_RigidBody(const std::unique_ptr<btRigidBody>& rigidbody);

		void Add_Ghost(std::weak_ptr<Collider> col, const std::unique_ptr<btGhostObject>& ghost, int layer);
		void Resize_Ghost(const std::unique_ptr<btGhostObject>& ghost);
		void Remove_Ghost(const std::unique_ptr<btGhostObject>& ghost);

		void Set_Debug_Drawer() const;
		void Render_Debug() const;

		std::unique_ptr<btDiscreteDynamicsWorld> world;

	private:
		std::unique_ptr<btDefaultCollisionConfiguration> collision_config;
		std::unique_ptr<btCollisionDispatcher> dispatcher;
		std::unique_ptr<btBroadphaseInterface> broadphase;
		std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
		std::unique_ptr<btGhostPairCallback> ghost_pair_call;

		std::unordered_map<const btCollisionObject*, std::weak_ptr<Collider>> collider_list;
	};
}