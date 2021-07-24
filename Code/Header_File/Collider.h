#pragma once
#include "Component.h"
#include <unordered_map>

namespace BeastEngine
{
	class MonoBehaviour;
	class Collider_Manager;

	class Collider :public BeastEngine::Component
	{
	public:
		void Set_Enabled(bool value); //表示するか
		bool Get_Enabled();			 //現在アクティブか

		bool is_trigger = false;

	protected:
		void Initialize_MonoBehaviour();

		bool is_called = false;
		bool enabled = true;
		bool enabled_old = false;

	private:
		bool Can_Multiple() override { return false; };
		void Call_Hit(std::shared_ptr<Collider>& col);
		void Call_Update();
		void Call_OnTrigger_Enter(std::shared_ptr<Collider>& col);
		void Call_OnCollision_Enter(std::shared_ptr<Collider>& col);
		void Call_OnTrigger_Stay(std::shared_ptr<Collider>& col);
		void Call_OnCollision_Stay(std::shared_ptr<Collider>& col);
		void Call_OnTrigger_Exit(std::shared_ptr<Collider>& col);
		void Call_OnCollision_Exit(std::shared_ptr<Collider>& col);

		std::vector<std::weak_ptr<BeastEngine::MonoBehaviour>> send_list;
		std::unordered_map<std::string, std::weak_ptr<BeastEngine::Collider>> hit_list;

		friend class BeastEngine::Collider_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Component>(this), enabled);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Collider)
CEREAL_CLASS_VERSION(BeastEngine::Collider, 1)