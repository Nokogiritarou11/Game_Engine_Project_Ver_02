#include "Animator_Controller.h"
#include "Animator_State_Machine.h"
#include "Animator_State_Transition.h"
#include "Transform.h"
#include "Time_Engine.h"
using namespace std;
using namespace BeastEngine;

template<class Archive>
void Animator_Controller::serialize(Archive& archive)
{
	archive(cereal::base_class<BeastEngine::Object>(this), state_machines, parameters);
}

void Animator_Controller::Initialize(shared_ptr<Transform> root)
{
	root_transform = root;
	parameters = make_shared<unordered_map<string, Controller_Parameter>>();
	for (auto state : state_machines)
	{
		state->Initialize(parameters);
		Set_Animation_Target(state);
		if (state->is_default_state)
		{
			playing_state_machine = state;
			playing_state_machine->Set_Active();
		}
	}
}

bool Animator_Controller::Add_State_Machine(string name)
{
	bool already = false;
	for (auto state : state_machines)
	{
		if (state->name == name) { already = true; return false; }
	}

	shared_ptr<Animator_State_Machine> state_machine = make_shared<Animator_State_Machine>();
	state_machine->name = name;
	state_machines.push_back(state_machine);
	Set_Animation_Target(state_machine);

	return true;
}

void Animator_Controller::Set_Animation_Target(shared_ptr<Animator_State_Machine> state)
{
	shared_ptr<Animation_Clip>& clip = state->clip;

	if (!clip->is_humanoid)
	{
		for (auto anim : clip->animations)
		{
			if (clip->is_humanoid)
			{
				//TODO:Avatarが確定したらやる
				//Animation_Target t = { Humanoid_Rig::None,t_trans, t_trans->Get_Local_Position(),t_trans->Get_Local_Euler_Angles(),t_trans->Get_Local_Scale() };
				//pose_default[anim.Target_Path] = t;
			}
			else
			{
				shared_ptr t_trans = root_transform.lock()->Find(anim.Target_Path).lock();
				if (t_trans)
				{
					auto it = pose_default.find(anim.Target_Path);
					if (it == pose_default.end())
					{
						Animation_Target t = { Humanoid_Rig::None,t_trans, t_trans->Get_Local_Position(),t_trans->Get_Local_Euler_Angles(),t_trans->Get_Local_Scale() };
						pose_default[anim.Target_Path] = t;
					}
				}
			}
		}
	}
}

void Animator_Controller::Update()
{
	if (!active_transition)
	{
		playing_state_machine->Update_Transition();
		playing_state_machine->Update_Time();

		if (playing_state_machine->transition_trigger)
		{
			duration_timer = 0;
			active_transition = playing_state_machine->Get_Active_Transition();
			next_state_machine = active_transition->next_state;
			next_state_machine->Set_Active(active_transition->transition_offset);
			playing_state_machine->transition_trigger = false;
		}
	}
	else
	{
		playing_state_machine->Update_Time();
		next_state_machine->Update_Time();
		duration_timer += Time::delta_time;

		if (duration_timer >= active_transition->transition_duration)
		{
			playing_state_machine->Exit();
			playing_state_machine = next_state_machine;
			pose_interrupt.clear();
			next_state_machine.reset();
			active_transition.reset();
			duration_timer = 0;
		}
		else
		{
			if (active_transition->interruption_source == Animator_State_Transition::Interruption_Source::Current_State)
			{
				playing_state_machine->Update_Transition();

				if (playing_state_machine->transition_trigger)
				{
					pose_interrupt = animation_data;
					next_state_machine->Exit();
					duration_timer = 0;
					active_transition = playing_state_machine->Get_Active_Transition();
					next_state_machine = active_transition->next_state;
					next_state_machine->Set_Active(active_transition->transition_offset);
					playing_state_machine->transition_trigger = false;
				}
			}
			else if (active_transition->interruption_source == Animator_State_Transition::Interruption_Source::Next_State)
			{
				next_state_machine->Update_Transition();

				if (next_state_machine->transition_trigger)
				{
					pose_interrupt = animation_data;
					playing_state_machine->Exit();
					playing_state_machine = next_state_machine;
					duration_timer = 0;
					active_transition = next_state_machine->Get_Active_Transition();
					next_state_machine = active_transition->next_state;
					next_state_machine->Set_Active(active_transition->transition_offset);
					next_state_machine->transition_trigger = false;
				}
			}
		}
	}

	pose_playing = pose_default;
	pose_next = pose_default;

	if (next_state_machine)
	{
		for (auto animation : playing_state_machine->clip->animations)
		{
			const vector<Animation_Clip::Keyframe>& keyframes = animation.keys;

			const int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// 現在の時間がどのキーフレームの間にいるか判定する
				const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
				const float& current_sec = playing_state_machine->currentSeconds;

				if (current_sec >= keyframe0.time && current_sec < keyframe1.time)
				{
					const float rate = (current_sec - keyframe0.time / keyframe1.time - keyframe0.time);

					Animation_Target& target = pose_playing[animation.Target_Path];
					// ２つのキーフレーム間の補完計算
					if (playing_state_machine->clip->is_humanoid)
					{
						target.humanoid_rig = animation.humanoid_type;
					}
					else
					{
						target.humanoid_rig = Humanoid_Rig::None;
					}
					target.position = Vector3::Lerp(keyframe0.position, keyframe1.position, rate);
					target.rotation = Quaternion::Slerp(keyframe0.rotation, keyframe1.rotation, rate);
					target.scale = Vector3::Lerp(keyframe0.scale, keyframe1.scale, rate);
					break;
				}
			}
		}
		for (auto animation : next_state_machine->clip->animations)
		{
			const vector<Animation_Clip::Keyframe>& keyframes = animation.keys;

			int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// 現在の時間がどのキーフレームの間にいるか判定する
				const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
				const float& current_sec = next_state_machine->currentSeconds;

				if (current_sec >= keyframe0.time && current_sec < keyframe1.time)
				{
					const float rate = (current_sec - keyframe0.time / keyframe1.time - keyframe0.time);

					Animation_Target& target = pose_next[animation.Target_Path];
					// ２つのキーフレーム間の補完計算
					if (next_state_machine->clip->is_humanoid)
					{
						target.humanoid_rig = animation.humanoid_type;
					}
					else
					{
						target.humanoid_rig = Humanoid_Rig::None;
					}
					target.position = Vector3::Lerp(keyframe0.position, keyframe1.position, rate);
					target.rotation = Quaternion::Slerp(keyframe0.rotation, keyframe1.rotation, rate);
					target.scale = Vector3::Lerp(keyframe0.scale, keyframe1.scale, rate);
					break;
				}
			}
		}

		if (playing_state_machine->clip->is_humanoid)
		{
			const float rate = duration_timer / active_transition->transition_duration;
			for (auto data : animation_data)
			{
				const Animation_Target& playing = pose_playing[data.first];
				const Animation_Target& next = pose_next[data.first];
				data.second.position = Vector3::Lerp(playing.position, next.position, rate);
				data.second.rotation = Quaternion::Slerp(playing.rotation, next.rotation, rate);
				data.second.scale = Vector3::Lerp(playing.scale, next.scale, rate);
			}
			if (!pose_interrupt.empty())
			{
				for (auto data : animation_data)
				{
					const Animation_Target& playing = pose_interrupt[data.first];
					const Animation_Target& next = data.second;
					data.second.position = Vector3::Lerp(playing.position, next.position, rate);
					data.second.rotation = Quaternion::Slerp(playing.rotation, next.rotation, rate);
					data.second.scale = Vector3::Lerp(playing.scale, next.scale, rate);
				}
			}
		}
	}
	else
	{
		for (auto animation : playing_state_machine->clip->animations)
		{
			const vector<Animation_Clip::Keyframe>& keyframes = animation.keys;

			const int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// 現在の時間がどのキーフレームの間にいるか判定する
				const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
				const float& current_sec = playing_state_machine->currentSeconds;

				if (current_sec >= keyframe0.time && current_sec < keyframe1.time)
				{
					const float rate = (current_sec - keyframe0.time / keyframe1.time - keyframe0.time);

					Animation_Target& target = pose_playing[animation.Target_Path];
					// ２つのキーフレーム間の補完計算
					if (playing_state_machine->clip->is_humanoid)
					{
						target.humanoid_rig = animation.humanoid_type;
					}
					else
					{
						target.humanoid_rig = Humanoid_Rig::None;
					}
					target.position = Vector3::Lerp(keyframe0.position, keyframe1.position, rate);
					target.rotation = Quaternion::Slerp(keyframe0.rotation, keyframe1.rotation, rate);
					target.scale = Vector3::Lerp(keyframe0.scale, keyframe1.scale, rate);
					break;
				}
			}
		}
		animation_data = pose_playing;
	}
}