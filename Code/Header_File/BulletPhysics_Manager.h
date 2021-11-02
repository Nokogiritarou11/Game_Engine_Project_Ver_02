#pragma once
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include <memory>
#include <unordered_map>

namespace BeastEngine
{
	class Collider;

	//BulletPhysics���C�u�����̃��b�p�[�������n�R���|�[�l���g�Ǘ��N���X
	class BulletPhysics_Manager
	{
	public:
		BulletPhysics_Manager();

		void Initialize(); //������
		void Reset();      //���Z�b�g����
		void Exit();       //��n��
		void Update();     //�X�V

		void Add_RigidBody(std::weak_ptr<Collider> col, const std::unique_ptr<btRigidBody>& rigidbody, int layer); //RigidBody��o�^����
		void Resize_RigidBody(const std::unique_ptr<btRigidBody>& rigidbody); //RigidBody�����T�C�Y����
		void Remove_RigidBody(const std::unique_ptr<btRigidBody>& rigidbody); //RigidBody���폜����

		void Add_Ghost(std::weak_ptr<Collider> col, const std::unique_ptr<btGhostObject>& ghost, int layer); //Ghost(Trigger)��o�^����
		void Resize_Ghost(const std::unique_ptr<btGhostObject>& ghost); //Ghost(Trigger)�����T�C�Y����
		void Remove_Ghost(const std::unique_ptr<btGhostObject>& ghost); //Ghost(Trigger)���폜����

		void Set_Debug_Drawer() const; //�f�o�b�O�\��(���C���[�t���[��)�ɓo�^����
		void Render_Debug() const; //�f�o�b�O�`��

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