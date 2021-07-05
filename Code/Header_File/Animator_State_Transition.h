#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include "Object.h"

namespace BeastEngine
{
	class Animator_State_Machine;

	class Animator_State_Transition
	{
	public:
		struct Condition_Base //ëJà⁄èåèåpè≥å≥
		{
			bool can_exit;
			template <class... Args>
			std::tuple<Args...> values;
			virtual int Get_Condition_Type() = 0;
			virtual void Update() = 0;
		};

		struct Condition_int : public Condition_Base
		{
			int Get_Condition_Type() override { return 0; };
			void Update() override;
		};

		enum class Interruption_Source
		{
			None,
			Current_State,
			Next_State,
		};

		bool has_exit_time;
		float exit_time;
		float transition_duration;
		float transition_offset;
		Interruption_Source interruption_source;
		std::unordered_map<std::string, Condition_Base> conditions;
		std::shared_ptr<Animator_State_Machine> next_state;

	private:

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(has_exit_time, exit_time, transition_duration, transition_offset, interruption_source, conditions, next_state);
		}
	};
}