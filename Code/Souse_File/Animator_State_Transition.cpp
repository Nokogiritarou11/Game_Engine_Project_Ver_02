#include "Animator_State_Transition.h"
#include "Animator_State_Machine.h"
#include "Transform.h"
using namespace std;
using namespace BeastEngine;

template<class Archive>
void Animator_State_Transition::serialize(Archive& archive)
{
	archive(has_exit_time, exit_time, transition_duration, transition_offset, interruption_source, conditions, next_state);
}

bool Animator_State_Transition::Check_Transition()
{
	bool exit = true;
	for (auto condition : conditions)
	{
		auto it = parameters->find(condition.key);
		if (it != parameters->end())
		{
			switch (condition.mode)
			{
				case Condition_Mode::If:
				{
					if (condition.type == Condition_Type::Bool || condition.type == Condition_Type::Trigger)
					{
						if (!it->second.value.value_bool) { exit = false; }
					}
				}
				break;

				case Condition_Mode::IfNot:
				{
					if (condition.type == Condition_Type::Bool)
					{
						if (it->second.value.value_bool) { exit = false; }
					}
				}
				break;

				case Condition_Mode::Greater:
				{
					if (condition.type == Condition_Type::Int)
					{
						if (it->second.value.value_int <= condition.threshold) { exit = false; }
					}
					if (condition.type == Condition_Type::Float)
					{
						if (it->second.value.value_float <= condition.threshold) { exit = false; }
					}
				}
				break;

				case Condition_Mode::Less:
				{
					if (condition.type == Condition_Type::Int)
					{
						if (it->second.value.value_int >= condition.threshold) { exit = false; }
					}
					if (condition.type == Condition_Type::Float)
					{
						if (it->second.value.value_float >= condition.threshold) { exit = false; }
					}
				}
				break;

				case Condition_Mode::Equals:
					break;
					{
						if (condition.type == Condition_Type::Int)
						{
							if (it->second.value.value_int != (int)condition.threshold) { exit = false; }
						}
					}

				case Condition_Mode::NotEquals:
					if (condition.type == Condition_Type::Int)
					{
						if (it->second.value.value_int == (int)condition.threshold) { exit = false; }
					}
					break;
			}
			if (!exit) break;
		}
	}

	can_transition = exit;
	return can_transition;
}

void Animator_State_Transition::Active()
{
	for (auto condition : conditions)
	{
		if (condition.type == Condition_Type::Trigger)
		{
			auto it = parameters->find(condition.key);
			if (it != parameters->end())
			{
				it->second.value.value_bool = false;
			}
		}
	}
}

void Animator_State_Transition::Add_Condition(string key, BeastEngine::Condition_Type type, Condition_Mode mode, float threshold)
{
	Condition condition = { key,type,mode,threshold };
	conditions.push_back(condition);
}

void Animator_State_Transition::Remove_Condition(int index)
{
	conditions.erase(conditions.begin() + index);
}