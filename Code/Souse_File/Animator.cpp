#include "Animator.h"
#include "Engine.h"
#include "Animator_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animation_Clip.h"
#include "Animator_State_Machine.h"
#include "Animator_Controller.h"
#include "Time_Engine.h"
#include "Include_ImGui.h"
#include "System_Function.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include "Debug.h"
using namespace std;
using namespace BeastEngine;

void Animator::Initialize(shared_ptr<GameObject> obj)
{
	Engine::asset_manager->Registration_Asset(shared_from_this());
	gameobject = obj;
	transform = obj->transform;
	Engine::animator_manager->Add(static_pointer_cast<Animator>(shared_from_this()));

	if (!controller_path.empty())
	{
		controller = Animator_Controller::Load_Animator_Controller(controller_path);
		controller->Initialize();
		Set_Default_Pose();
	}
}

void Animator::Set_Default_Pose()
{
	for (auto state : controller->state_machines)
	{
		if (state->clip)
		{
			shared_ptr<Animation_Clip>& clip = state->clip;

			for (auto& anim : clip->animations)
			{
				shared_ptr t_trans = transform->Find(anim.Target_Path).lock();
				if (t_trans)
				{
					auto it = pose_default.find(anim.Target_Path);
					if (it == pose_default.end())
					{
						Animation_Target t = { t_trans, t_trans->Get_Local_Position(),t_trans->Get_Local_Rotation(),t_trans->Get_Local_Scale() };
						pose_default[anim.Target_Path] = t;
					}
				}
			}
			pose_playing = pose_default;
			pose_next = pose_default;
			pose_interrupt.clear();
		}
	}
}

void Animator::Set_Int(string key, int value)
{
	auto it = controller->parameters->find(key);
	if (it != controller->parameters->end())
	{
		it->second.value_int = value;
	}
}
void Animator::Set_Float(string key, float value)
{
	auto it = controller->parameters->find(key);
	if (it != controller->parameters->end())
	{
		it->second.value_float = value;
	}
}
void Animator::Set_Bool(string key, bool value)
{
	auto it = controller->parameters->find(key);
	if (it != controller->parameters->end())
	{
		it->second.value_bool = value;
	}
}
void Animator::Set_Trigger(string key)
{
	auto it = controller->parameters->find(key);
	if (it != controller->parameters->end())
	{
		it->second.value_bool = true;
	}
}
void Animator::Reset_Trigger(string key)
{
	auto it = controller->parameters->find(key);
	if (it != controller->parameters->end())
	{
		it->second.value_bool = false;
	}
}

void Animator::Update()
{
	if (!playing) return;

	controller->Update();

	switch (controller->interrupt_state)
	{
		case 1:
			pose_interrupt.clear();
			controller->interrupt_state = 0;
			break;
		case 2:
			pose_interrupt = animation_data;
			controller->interrupt_state = 0;
			break;
	}

	pose_playing = pose_default;
	pose_next = pose_default;
	animation_data = pose_default;

	if (controller->next_state_machine)
	{
		const float& current_sec = controller->playing_state_machine->currentSeconds;
		for (auto& animation : controller->playing_state_machine->clip->animations)
		{
			const vector<Animation_Clip::Keyframe>& keyframes = animation.keys;

			const int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// 現在の時間がどのキーフレームの間にいるか判定する
				const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

				if (current_sec >= keyframe0.time && current_sec < keyframe1.time)
				{
					const float rate = (current_sec - keyframe0.time) / (keyframe1.time - keyframe0.time);

					Animation_Target& target = pose_playing[animation.Target_Path];
					// ２つのキーフレーム間の補完計算
					target.position = Vector3::Lerp(keyframe0.position, keyframe1.position, rate);
					target.rotation = Quaternion::Slerp(keyframe0.rotation, keyframe1.rotation, rate);
					target.scale = Vector3::Lerp(keyframe0.scale, keyframe1.scale, rate);
					break;
				}
			}
		}

		const float& next_sec = controller->next_state_machine->currentSeconds;
		for (auto& animation : controller->next_state_machine->clip->animations)
		{
			const vector<Animation_Clip::Keyframe>& keyframes = animation.keys;

			int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// 現在の時間がどのキーフレームの間にいるか判定する
				const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

				if (next_sec >= keyframe0.time && next_sec < keyframe1.time)
				{
					const float rate = (next_sec - keyframe0.time) / (keyframe1.time - keyframe0.time);

					Animation_Target& target = pose_next[animation.Target_Path];
					// ２つのキーフレーム間の補完計算
					target.position = Vector3::Lerp(keyframe0.position, keyframe1.position, rate);
					target.rotation = Quaternion::Slerp(keyframe0.rotation, keyframe1.rotation, rate);
					target.scale = Vector3::Lerp(keyframe0.scale, keyframe1.scale, rate);
					break;
				}
			}
		}

		const float rate = controller->duration_timer / controller->active_transition->transition_duration;
		for (auto& data : animation_data)
		{
			const Animation_Target& playing = pose_playing[data.first];
			const Animation_Target& next = pose_next[data.first];
			data.second.position = Vector3::Lerp(playing.position, next.position, rate);
			data.second.rotation = Quaternion::Slerp(playing.rotation, next.rotation, rate);
			data.second.scale = Vector3::Lerp(playing.scale, next.scale, rate);
		}
		if (!pose_interrupt.empty())
		{
			for (auto& data : animation_data)
			{
				const Animation_Target& playing = pose_interrupt[data.first];
				const Animation_Target& next = data.second;
				data.second.position = Vector3::Lerp(playing.position, next.position, rate);
				data.second.rotation = Quaternion::Slerp(playing.rotation, next.rotation, rate);
				data.second.scale = Vector3::Lerp(playing.scale, next.scale, rate);
			}
		}
	}
	else
	{
		const float& current_sec = controller->playing_state_machine->currentSeconds;
		for (auto& animation : controller->playing_state_machine->clip->animations)
		{
			const vector<Animation_Clip::Keyframe>& keyframes = animation.keys;

			const int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// 現在の時間がどのキーフレームの間にいるか判定する
				const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

				if (current_sec >= keyframe0.time && current_sec < keyframe1.time)
				{
					const float rate = (current_sec - keyframe0.time) / (keyframe1.time - keyframe0.time);

					Animation_Target& target = pose_playing[animation.Target_Path];
					// ２つのキーフレーム間の補完計算
					target.position = Vector3::Lerp(keyframe0.position, keyframe1.position, rate);
					target.rotation = Quaternion::Slerp(keyframe0.rotation, keyframe1.rotation, rate);
					target.scale = Vector3::Lerp(keyframe0.scale, keyframe1.scale, rate);
					break;
				}
			}
		}
		animation_data = pose_playing;
	}

	for (auto& data : animation_data)
	{
		Animation_Target& anim = data.second;
		if (shared_ptr<Transform> target = anim.target.lock())
		{
			target->Set_Local_Position(anim.position);
			target->Set_Local_Rotation(anim.rotation);
			target->Set_Local_Scale(anim.scale);
		}
	}
}

// アニメーション再生
void Animator::Play()
{
	playing = true;
}

void Animator::Stop()
{
	playing = false;
}

void Animator::Pause()
{
	playing = false;
}

bool Animator::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Animator_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Animator>(shared_from_this()));
			removed = false;
		}
		ImGui::EndPopup();
		if (!removed)
		{
			return false;
		}
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
	bool enable = Get_Enabled();
	if (ImGui::Checkbox("##enable", &enable))
	{
		Set_Enabled(enable);
	}

	if (open)
	{
		if (controller)
		{
			string controller_name = controller->name + ".controller";
			ImGui::Text(controller_name.c_str());
		}
		else
		{
			ImGui::Text(u8"アニメーターコントローラーが登録されていません");
		}
		ImGui::Spacing();
		if (ImGui::Button(u8"新規作成"))
		{
			controller = Animator_Controller::Create_New_Controller();
			if (controller)
			{
				controller_path = controller->save_path;
				controller->Initialize();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"読み込む"))
		{
			controller = Animator_Controller::Load_Animator_Controller();
			if (controller)
			{
				controller_path = controller->save_path;
				controller->Initialize();
				Set_Default_Pose();
			}
		}
	}
	return true;
}