#include "Animator_State_Transition.h"
#include "Animator_State_Machine.h"
#include "Transform.h"
using namespace std;
using namespace BeastEngine;

void Animator_State_Transition::Initialize(std::shared_ptr<std::unordered_map<std::string, BeastEngine::Animation_Parameter>>& p_parameters, std::shared_ptr<BeastEngine::Animator_State_Machine>& next)
{
	parameters = p_parameters;
	next_state = next;
}

bool Animator_State_Transition::Check_Transition()
{
	bool exit = true;
	for (auto& condition : conditions)
	{
		auto it = parameters->find(condition->key);
		if (it != parameters->end())
		{
			switch (condition->mode)
			{
				case Condition_Mode::If:
				{
					if (condition->type == Parameter_Type::Bool || condition->type == Parameter_Type::Trigger)
					{
						if (!it->second.value_bool) { exit = false; }
					}
				}
				break;

				case Condition_Mode::IfNot:
				{
					if (condition->type == Parameter_Type::Bool)
					{
						if (it->second.value_bool) { exit = false; }
					}
				}
				break;

				case Condition_Mode::Greater:
				{
					if (condition->type == Parameter_Type::Int)
					{
						if (it->second.value_int <= condition->threshold) { exit = false; }
					}
					if (condition->type == Parameter_Type::Float)
					{
						if (it->second.value_float <= condition->threshold) { exit = false; }
					}
				}
				break;

				case Condition_Mode::Less:
				{
					if (condition->type == Parameter_Type::Int)
					{
						if (it->second.value_int >= condition->threshold) { exit = false; }
					}
					if (condition->type == Parameter_Type::Float)
					{
						if (it->second.value_float >= condition->threshold) { exit = false; }
					}
				}
				break;

				case Condition_Mode::Equals:
					break;
					{
						if (condition->type == Parameter_Type::Int)
						{
							if (it->second.value_int != (int)condition->threshold) { exit = false; }
						}
					}

				case Condition_Mode::NotEquals:
					if (condition->type == Parameter_Type::Int)
					{
						if (it->second.value_int == (int)condition->threshold) { exit = false; }
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
	for (auto& condition : conditions)
	{
		if (condition->type == Parameter_Type::Trigger)
		{
			auto it = parameters->find(condition->key);
			if (it != parameters->end())
			{
				it->second.value_bool = false;
			}
		}
	}
}

void Animator_State_Transition::Add_Condition(string key, Parameter_Type type, Condition_Mode mode, float threshold)
{
	shared_ptr<Condition> condition = make_shared<Condition>();
	condition->key = key;
	condition->type = type;
	condition->mode = mode;
	condition->threshold = threshold;
	conditions.emplace_back(condition);
}

void Animator_State_Transition::Remove_Condition(int index)
{
	conditions.erase(conditions.begin() + index);
}