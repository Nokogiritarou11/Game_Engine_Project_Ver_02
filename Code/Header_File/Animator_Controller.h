#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Object.h"

namespace BeastEngine
{
	class Animator_State_Machine;

	class Animator_Controller : public BeastEngine::Object
	{
	public:
		std::vector<std::shared_ptr<BeastEngine::Animator_State_Machine>> state_machines;

	private:

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Object>(this), state_machines);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Animator_Controller)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Animator_Controller)