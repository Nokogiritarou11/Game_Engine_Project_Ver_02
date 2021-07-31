#pragma once
//DirectMathÇ∆ÇÃèdï°âÒîóp
//#define BT_NO_SIMD_OPERATOR_OVERLOADS
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btIDebugDraw.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include "Original_Math.h"

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

		void Add_RigidBody(std::weak_ptr<BeastEngine::Collider> col, std::unique_ptr<btRigidBody>& rigidbody, int layer);
		void Resize_RigidBody(std::unique_ptr<btRigidBody>& rigidbody);
		void Remove_RigidBody(std::unique_ptr<btRigidBody>& rigidbody);

		void Add_Ghost(std::weak_ptr<BeastEngine::Collider> col, std::unique_ptr<btGhostObject>& ghost, int layer);
		void Resize_Ghost(std::unique_ptr<btGhostObject>& ghost);
		void Remove_Ghost(std::unique_ptr<btGhostObject>& ghost);

		void Set_Debug_Drawer();
		void Render_Debug();

	private:
		std::unique_ptr<btDefaultCollisionConfiguration> collision_config;
		std::unique_ptr<btCollisionDispatcher> dispatcher;
		std::unique_ptr<btBroadphaseInterface> broadphase;
		std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
		std::unique_ptr<btGhostPairCallback> ghost_pair_call;
		std::unique_ptr<btDiscreteDynamicsWorld> world;

		std::unordered_map<const btCollisionObject*, std::weak_ptr<Collider>> collider_list;
	};
}