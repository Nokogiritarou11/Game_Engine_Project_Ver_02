#pragma once
#include <unordered_map>
#include "btBulletDynamicsCommon.h"
#include "Component.h"
#include "RigidBody.h"
#include "GhostObject.h"

namespace BeastEngine
{
	class MonoBehaviour;
	class BulletPhysics_Manager;
	class Editor;

	struct Collision
	{
		const std::shared_ptr<Collider> collider;
		const std::shared_ptr<GameObject> gameobject;
		const std::shared_ptr<Transform> transform;
		const std::vector<Vector3> contacts{};
	};

	class Collider : public Component
	{
	public:
		void Set_Enabled(bool value); //表示するか
		bool Get_Enabled() const;			  //現在アクティブか
		void Set_IsTrigger(bool value);
		std::shared_ptr<RigidBody> rigidbody;
		Vector3 center;

	protected:
		virtual void Create_Shape() {};
		void Rescale_Shape() const;
		void Create_Collider();
		void Reset_Collider();

		void Initialize_MonoBehaviour();
		void Draw_ImGui_Common();
		bool Draw_ImGui_Header(const std::string& component_name, bool& open);

		std::unique_ptr<btCollisionShape> shape;
		std::unique_ptr<GhostObject> ghost;
		bool is_trigger = false;

	private:
		void Initialize(std::shared_ptr<GameObject> obj) override;
		void Set_Active(bool value) override;
		bool Can_Multiple() override { return false; };

		void Update_Transform();
		void Update_Simulation();

		void Call_Hit(Collision& collision);

		void Call_OnTrigger_Enter(Collision& collision);
		void Call_OnTrigger_Stay(Collision& collision);
		void Call_OnTrigger_Exit(Collision& collision);

		void Call_OnCollision_Enter(Collision& collision);
		void Call_OnCollision_Stay(Collision& collision);
		void Call_OnCollision_Exit(Collision& collision);

		void Set_Debug_Draw(bool value) const;

		std::vector<std::weak_ptr<MonoBehaviour>> send_list;
		std::unordered_map<std::string, std::weak_ptr<Collider>> hit_list;
		std::unordered_map<std::string, std::weak_ptr<Collider>> hit_list_old;

		bool is_called = false;
		bool enabled = true;
		bool enabled_old = false;
		bool disabled = false;
		bool disabled_old = false;
		bool debug_drawed = false;

		Vector3 position_old;
		Quaternion rotation_old;

		friend class Editor;
		friend class BulletPhysics_Manager;
		friend class RigidBody;
		friend class GhostObject;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Component>(this), enabled, is_trigger, rigidbody, center);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Collider)
CEREAL_CLASS_VERSION(BeastEngine::Collider, 1)