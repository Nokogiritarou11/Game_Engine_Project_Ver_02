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

		bool has_exit_time = true;
		bool exit_trigger = false;
		float exit_time = 1.0f;
		float transition_duration = 0;
		float transition_offset = 0;
		Interruption_Source interruption_source = Interruption_Source::None;
		std::vector<BeastEngine::Condition> conditions;
		std::shared_ptr<BeastEngine::Animator_State_Machine> next_state;
		std::shared_ptr<std::unordered_map<std::string, Controller_Parameter>> parameters;

		bool Check_Transition();
		void Active();
		void Add_Condition(std::string key, BeastEngine::Condition_Type type, BeastEngine::Condition_Mode mode, float threshold);
		void Remove_Condition(int index);
		bool Get_Can_Transition() { return can_transition; }

	private:
		bool can_transition = false;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive);
	};
}