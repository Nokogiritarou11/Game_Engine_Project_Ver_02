#include "Animator_State_Machine.h"
#include "Transform.h"
#include "Time_Engine.h"
#include "Debug.h"
using namespace std;
using namespace BeastEngine;

void Animator_State_Machine::Initialize(const shared_ptr<unordered_map<string, Animation_Parameter>>& p_parameters)
{
	parameters = p_parameters;

	if (!path.empty())
	{
		Set_Clip(path);
	}
	for (const auto& transition : transitions)
	{
		transition->parameters = p_parameters;
	}
}

void Animator_State_Machine::Reset()
{
	transition_trigger = false;
	active_transition.reset();
	is_end_animation = false;
	current_seconds = 0;

	for (const auto& transition : transitions)
	{
		transition->exit_called = false;
		transition->exit_trigger = false;
	}
	for (auto& eve : animation_events)
	{
		eve.called = false;
	}
}

void Animator_State_Machine::Set_Clip(const string& full_path)
{
	if (const auto& new_clip = Animation_Clip::Load_Clip(full_path))
	{
		if (clip)
		{
			end_frame = -1;
		}

		if (end_frame < 0)
		{
			end_frame = new_clip->Get_Frame_Count();
		}
		clip = new_clip;
		path = full_path;
	}
}

void Animator_State_Machine::Activate(float transition_offset)
{
	is_end_animation = false;
	const float end_sec = static_cast<float>(end_frame) * 0.0166666666666667f;
	const float start_sec = static_cast<float>(start_frame) * 0.0166666666666667f;
	current_seconds = transition_offset * (end_sec - start_sec) + start_sec;

	for (auto& eve : state_events)
	{
		if (eve.type == State_Event_Type::Enter)
		{
			auto it = parameters->find(eve.key);
			if (it != parameters->end())
			{
				switch (eve.parameter.type)
				{
					case Parameter_Type::Int:
						if (it->second.type == Parameter_Type::Int)
						{
							it->second.value_int = eve.parameter.value_int;
						}
						else
						{
							Debug::Log(u8"Eventで指定されたパラメーターの型が一致しません");
						}
						break;

					case Parameter_Type::Float:
						if (it->second.type == Parameter_Type::Float)
						{
							it->second.value_float = eve.parameter.value_float;
						}
						else
						{
							Debug::Log(u8"Eventで指定されたパラメーターの型が一致しません");
						}
						break;

					case Parameter_Type::Bool:
						if (it->second.type == Parameter_Type::Bool)
						{
							it->second.value_bool = eve.parameter.value_bool;
						}
						else
						{
							Debug::Log(u8"Eventで指定されたパラメーターの型が一致しません");
						}
						break;

					case Parameter_Type::Trigger:
						if (it->second.type == Parameter_Type::Trigger)
						{
							it->second.value_bool = eve.parameter.value_bool;
						}
						else
						{
							Debug::Log(u8"Eventで指定されたパラメーターの型が一致しません");
						}
						break;
				}
			}
			else
			{
				Debug::Log(u8"Eventで指定されたパラメーターが見つかりません");
			}
		}
	}
}

void Animator_State_Machine::Exit()
{
	Reset();

	for (auto& eve : state_events)
	{
		if (eve.type == State_Event_Type::Exit)
		{
			auto it = parameters->find(eve.key);
			if (it != parameters->end())
			{
				switch (eve.parameter.type)
				{
					case Parameter_Type::Int:
						if (it->second.type == Parameter_Type::Int)
						{
							it->second.value_int = eve.parameter.value_int;
						}
						else
						{
							Debug::Log(u8"Eventで指定されたパラメーターの型が一致しません");
						}
						break;

					case Parameter_Type::Float:
						if (it->second.type == Parameter_Type::Float)
						{
							it->second.value_float = eve.parameter.value_float;
						}
						else
						{
							Debug::Log(u8"Eventで指定されたパラメーターの型が一致しません");
						}
						break;

					case Parameter_Type::Bool:
						if (it->second.type == Parameter_Type::Bool)
						{
							it->second.value_bool = eve.parameter.value_bool;
						}
						else
						{
							Debug::Log(u8"Eventで指定されたパラメーターの型が一致しません");
						}
						break;

					case Parameter_Type::Trigger:
						if (it->second.type == Parameter_Type::Trigger)
						{
							it->second.value_bool = eve.parameter.value_bool;
						}
						else
						{
							Debug::Log(u8"Eventで指定されたパラメーターの型が一致しません");
						}
						break;
				}
			}
			else
			{
				Debug::Log(u8"Eventで指定されたパラメーターが見つかりません");
			}
		}
	}
}

void Animator_State_Machine::Update_Transition()
{
	bool exit = false;
	size_t i = 0;
	for (const auto& transition : transitions)
	{
		if (transition->has_exit_time)
		{
			if (transition->exit_trigger)
			{
				exit = transition->Check_Transition();
				transition->exit_trigger = false;
				transition->exit_called = true;
				if (exit) break;
			}
		}
		else
		{
			exit = transition->Check_Transition();
			if (exit) break;
		}
		++i;
	}

	if (exit)
	{
		transition_trigger = true;
		active_transition = transitions[i];
	}

}

void Animator_State_Machine::Update_Time(const float& speed)
{
	const float end_sec = static_cast<float>(end_frame) * 0.0166666666666667f;

	// 最終フレーム処理
	if (is_end_animation)
	{
		current_seconds = end_sec;
		return;
	}

	float multiplier = speed;
	// 時間経過
	if (use_speed_multiplier && !multiplier_hash.empty())
	{
		if (const auto it = parameters->find(multiplier_hash); it != parameters->end())
		{
			multiplier *= (*parameters)[multiplier_hash].value_float;
		}
	}
	current_seconds += Time::delta_time * animation_speed * multiplier;

	for (const auto& transition : transitions)
	{
		if (transition->has_exit_time && !transition->exit_called && current_seconds >= end_sec * transition->exit_time)
		{
			transition->exit_trigger = true;
		}
	}

	for (auto& eve : animation_events)
	{
		const float frame_time = eve.frame * 0.0166666666666667f;
		if (!eve.called && current_seconds >= frame_time)
		{
			auto it = parameters->find(eve.key);
			if (it != parameters->end())
			{
				switch (eve.parameter.type)
				{
					case Parameter_Type::Int:
						if (it->second.type == Parameter_Type::Int)
						{
							it->second.value_int = eve.parameter.value_int;
						}
						else
						{
							Debug::Log(u8"Eventで指定されたパラメーターの型が一致しません");
						}
						break;

					case Parameter_Type::Float:
						if (it->second.type == Parameter_Type::Float)
						{
							it->second.value_float = eve.parameter.value_float;
						}
						else
						{
							Debug::Log(u8"Eventで指定されたパラメーターの型が一致しません");
						}
						break;

					case Parameter_Type::Bool:
						if (it->second.type == Parameter_Type::Bool)
						{
							it->second.value_bool = eve.parameter.value_bool;
						}
						else
						{
							Debug::Log(u8"Eventで指定されたパラメーターの型が一致しません");
						}
						break;

					case Parameter_Type::Trigger:
						if (it->second.type == Parameter_Type::Trigger)
						{
							it->second.value_bool = eve.parameter.value_bool;
						}
						else
						{
							Debug::Log(u8"Eventで指定されたパラメーターの型が一致しません");
						}
						break;
				}
			}
			else
			{
				Debug::Log(u8"Eventで指定されたパラメーターが見つかりません");
			}
			eve.called = true;
		}
	}

	if (current_seconds >= end_sec)
	{
		for (const auto& transition : transitions)
		{
			transition->exit_called = false;
		}
		for (auto& eve : animation_events)
		{
			eve.called = false;
		}

		if (is_loop_animation)
		{
			const float start_sec = static_cast<float>(start_frame) * 0.0166666666666667f;
			current_seconds -= (end_sec - start_sec);
		}
		else
		{
			current_seconds = end_sec;
			is_end_animation = true;
		}
	}
}

void Animator_State_Machine::Add_Transition(shared_ptr<Animator_State_Machine>& next_state)
{
	shared_ptr<Animator_State_Transition> transition = make_shared<Animator_State_Transition>();
	transition->Initialize(parameters, next_state);
	transitions.emplace_back(transition);
}

void Animator_State_Machine::Remove_Transition(int index)
{
	transitions.erase(transitions.begin() + index);
}

void Animator_State_Machine::Add_Animation_Event()
{
	Animation_Event eve = {};
	animation_events.emplace_back(eve);
}

void Animator_State_Machine::Add_State_Event()
{
	State_Event eve = {};
	state_events.emplace_back(eve);
}

shared_ptr<Animator_State_Transition> Animator_State_Machine::Get_Active_Transition() const
{
	active_transition->Activate();
	return active_transition;
}