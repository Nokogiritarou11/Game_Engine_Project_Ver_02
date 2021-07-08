#include "Animator_State_Transition.h"
#include "Animator_State_Machine.h"
using namespace std;
using namespace BeastEngine;

bool Animator_State_Transition::Check_Transition(const unordered_map<string, Controller_Parameter>& parameter_map)
{
	bool exit = true;
	for (size_t i = 0; i < conditions.size(); ++i)
	{
		Condition condition = conditions[i];
		auto it = parameter_map.find(condition.key);
		if (it != parameter_map.end())
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

void Animator_State_Transition::Add_Condition(string key, BeastEngine::Condition_Type type, Condition_Mode mode, float threshold)
{
	Condition condition = { key,type,mode,threshold };
	conditions.push_back(condition);
}

void Animator_State_Transition::Remove_Condition(int index)
{
	conditions.erase(conditions.begin() + index);
}