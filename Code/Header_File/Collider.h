#pragma once
#include <unordered_map>
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btIDebugDraw.h"
#include "Component.h"
#include "RigidBody.h"

namespace BeastEngine
{
	class MonoBehaviour;
	class BulletPhysics_Manager;

	struct Collision
	{
		const std::shared_ptr<BeastEngine::Collider> collider;
		const std::shared_ptr<BeastEngine::GameObject> gameobject;
		const std::shared_ptr<BeastEngine::Transform> transform;
		const std::vector<BeastEngine::Vector3> contacts;
	};

	class Collider : public BeastEngine::Component
	{
	public:
		~Collider();
		void Set_Enabled(bool value); //表示するか
		bool Get_Enabled();			  //現在アクティブか
		void Set_IsTrigger(bool value);
		std::shared_ptr<RigidBody> rigidbody;

	protected:
		void Initialize_MonoBehaviour();

		std::unique_ptr<btCollisionShape> shape;
		std::unique_ptr<btGhostObject> ghost;
		bool is_trigger = false;

	private:
		virtual void Create_Shape() {};
		virtual void Resize_Shape() {};
		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		void Set_Active(bool value) override;
		bool Can_Multiple() override { return false; };

		void Create_Ghost();
		void Create_Collider();

		void Call_Hit(Collision& collision);
		void Call_Update();

		void Call_OnTrigger_Enter(BeastEngine::Collision& collision);
		void Call_OnTrigger_Stay(BeastEngine::Collision& collision);
		void Call_OnTrigger_Exit(BeastEngine::Collision& collision);

		void Call_OnCollision_Enter(BeastEngine::Collision& collision);
		void Call_OnCollision_Stay(BeastEngine::Collision& collision);
		void Call_OnCollision_Exit(BeastEngine::Collision& collision);

		std::vector<std::weak_ptr<BeastEngine::MonoBehaviour>> send_list;
		std::unordered_map<std::string, std::weak_ptr<BeastEngine::Collider>> hit_list;

		bool is_called = false;
		bool enabled = true;
		bool enabled_old = false;
		bool disabled = false;
		bool disabled_old = false;

		friend class BeastEngine::BulletPhysics_Manager;
		friend class BeastEngine::RigidBody;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Component>(this), enabled, is_trigger, rigidbody);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Collider)
CEREAL_CLASS_VERSION(BeastEngine::Collider, 1)