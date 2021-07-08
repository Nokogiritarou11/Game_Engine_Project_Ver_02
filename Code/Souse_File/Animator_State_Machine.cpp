#include "Animator_State_Machine.h"
#include "Transform.h"
#include "Time_Engine.h"
using namespace std;
using namespace BeastEngine;

void Animator_State_Machine::Initialize(shared_ptr<Transform> root)
{
	shared_ptr<Animation_Clip> clip = Animation_Clip::Load_Clip(path);

	Clip_Data data;
	data.clip = clip;

	for (size_t i = 0; i < clip->animations.size(); ++i)
	{
		Animation_Clip::Animation& anim = clip->animations[i];
		std::weak_ptr<Transform> t_trans;
		Humanoid_Rig humanoid_type = Humanoid_Rig::None;
		if (!clip->is_humanoid)
		{
			t_trans = root->Find(anim.Target_Path);
		}
		else
		{
			humanoid_type = anim.humanoid_type;
		}
		Animation_Target target = { humanoid_type, t_trans, anim };
		data.targets.push_back(target);
	}
}

void Animator_State_Machine::Update(const unordered_map<string, Controller_Parameter>& parameter_map)
{
	bool exit = false;
	for (size_t i = 0; i < transitions.size(); ++i)
	{
		exit = transitions[i].Check_Transition(parameter_map);
		if(!exit) break;
	}

	if (exit)
	{

	}

	// 最終フレーム処理
	if (endAnimation)
	{
		endAnimation = false;
		currentSeconds = 0;
		return;
	}

	// 時間経過
	currentSeconds += Time::delta_time * animation_speed;
	float length = clip_data.clip->Get_Length();
	if (currentSeconds >= length)
	{
		if (loopAnimation)
		{
			currentSeconds -= length;
		}
		else
		{
			currentSeconds = length;
			endAnimation = true;
		}
	}
}

void Animator_State_Machine::Add_Transition(shared_ptr<Animator_State_Machine> next_state)
{
	Animator_State_Transition transition;
	transition.next_state = next_state;
	transitions.push_back(transition);
}

void Animator_State_Machine::Remove_Transition(int index)
{
	transitions.erase(transitions.begin() + index);
}