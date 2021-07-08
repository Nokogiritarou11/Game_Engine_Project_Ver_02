#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Object.h"
#include "Animator_Condition.h"

namespace BeastEngine
{
	class Animator_State_Machine;
	class Transform;

	class Animator_Controller : public BeastEngine::Object
	{
	public:
		std::vector<std::shared_ptr<BeastEngine::Animator_State_Machine>> state_machines;
		std::shared_ptr<BeastEngine::Animator_State_Machine> playing_state_machine;

		void Initialize(std::shared_ptr<BeastEngine::Transform> root);
		void Update();

		std::unordered_map<std::string, Controller_Parameter> parameters;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Object>(this), state_machines);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Animator_Controller)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Animator_Controller)