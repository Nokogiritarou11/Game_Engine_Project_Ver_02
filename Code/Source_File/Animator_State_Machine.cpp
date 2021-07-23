#include "Animator_State_Machine.h"
#include "Transform.h"
#include "Time_Engine.h"
using namespace std;
using namespace BeastEngine;

void Animator_State_Machine::Initialize(shared_ptr<unordered_map<string, Animation_Parameter>>& p_parameters)
{
	parameters = p_parameters;
	if (!path.empty())
	{
		Set_Clip(path);
	}
	for (auto& transition : transitions)
	{
		transition->parameters = p_parameters;
	}
}

void Animator_State_Machine::Set_Clip(string fullpath)
{
	if (clip = Animation_Clip::Load_Clip(fullpath))
	{
		path = fullpath;
	}
}

void Animator_State_Machine::Set_Active(float transition_offset)
{
	endAnimation = false;
	currentSeconds = transition_offset * clip->Get_Length();
}

void Animator_State_Machine::Exit()
{
	transition_trigger = false;
	active_transition.reset();
	endAnimation = false;
	currentSeconds = 0;
}

void Animator_State_Machine::Update_Transition()
{
	bool exit = false;
	size_t i = 0;
	for (auto& transition : transitions)
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

void Animator_State_Machine::Update_Time()
{
	// 最終フレーム処理
	if (endAnimation)
	{
		endAnimation = false;
		currentSeconds = clip->Get_Length();
		return;
	}

	float multiplier = 1;
	// 時間経過
	if (!multiplier_hash.empty())
	{
		auto it = parameters->find(multiplier_hash);
		if (it != parameters->end())
		{
			multiplier = (*parameters)[multiplier_hash].value_float;
		}
	}
	currentSeconds += Time::delta_time * animation_speed * multiplier;

	for (auto& transition : transitions)
	{
		if (!transition->exit_called && currentSeconds >= clip->Get_Length() * transition->exit_time)
		{
			transition->exit_trigger = true;
		}
	}

	for (auto& eve : events)
	{
		if (!eve.called && currentSeconds >= eve.time)
		{
			eve.trigger = true;
		}
	}

	if (currentSeconds >= clip->Get_Length())
	{
		for (auto& transition : transitions)
		{
			transition->exit_called = false;
		}
		for (auto& eve : events)
		{
			eve.called = false;
		}

		if (loopAnimation)
		{
			currentSeconds -= clip->Get_Length();
		}
		else
		{
			currentSeconds = clip->Get_Length();
			endAnimation = true;
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

void Animator_State_Machine::Add_Event()
{
	Animation_Event eve = {};
	events.emplace_back(eve);
}

shared_ptr<BeastEngine::Animator_State_Transition> Animator_State_Machine::Get_Active_Transition()
{
	active_transition->Active();
	return active_transition;
}