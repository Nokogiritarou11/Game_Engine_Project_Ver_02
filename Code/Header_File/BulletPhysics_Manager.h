#pragma once
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include <memory>
#include <unordered_map>

namespace BeastEngine
{
	class Collider;

	//BulletPhysicsライブラリのラッパー兼物理系コンポーネント管理クラス
	class BulletPhysics_Manager
	{
	public:
		BulletPhysics_Manager();

		void Initialize(); //初期化
		void Reset();      //リセットする
		void Exit();       //後始末
		void Update();     //更新

		void Add_RigidBody(std::weak_ptr<Collider> col, const std::unique_ptr<btRigidBody>& rigidbody, int layer); //RigidBodyを登録する
		void Resize_RigidBody(const std::unique_ptr<btRigidBody>& rigidbody); //RigidBodyをリサイズする
		void Remove_RigidBody(const std::unique_ptr<btRigidBody>& rigidbody); //RigidBodyを削除する

		void Add_Ghost(std::weak_ptr<Collider> col, const std::unique_ptr<btGhostObject>& ghost, int layer); //Ghost(Trigger)を登録する
		void Resize_Ghost(const std::unique_ptr<btGhostObject>& ghost); //Ghost(Trigger)をリサイズする
		void Remove_Ghost(const std::unique_ptr<btGhostObject>& ghost); //Ghost(Trigger)を削除する

		void Set_Debug_Drawer() const; //デバッグ表示(ワイヤーフレーム)に登録する
		void Render_Debug() const; //デバッグ描画

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