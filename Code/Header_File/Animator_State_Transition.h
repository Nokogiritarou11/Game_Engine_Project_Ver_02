#pragma once
#include <vector>
#include <unordered_map>
#include <utility>
#include <memory>
#include "Animator_Condition.h"

namespace BeastEngine
{
	class Animator_State_Machine;

	class Animator_State_Transition
	{
	public:
		enum class Interruption_Source
		{
			None,
			Current_State,
			Next_State,
		};

		bool has_exit_time = false;
		float exit_time = 1.0f;
		float transition_duration = 0;
		float transition_offset = 0;
		Interruption_Source interruption_source = Interruption_Source::None;
		std::vector<BeastEngine::Condition> conditions;
		std::shared_ptr<BeastEngine::Animator_State_Machine> next_state;
		std::string next_state_path;

		bool Check_Transition(const std::unordered_map<std::string, Controller_Parameter>& parameter_map);
		void Add_Condition(std::string key, BeastEngine::Condition_Type type, BeastEngine::Condition_Mode mode, float threshold);
		void Remove_Condition(int index);
		bool Get_Can_Transition() { return can_transition; }

	private:
		bool can_transition;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(has_exit_time, exit_time, transition_duration, transition_offset, interruption_source, conditions, next_state_path);
		}
	};
}