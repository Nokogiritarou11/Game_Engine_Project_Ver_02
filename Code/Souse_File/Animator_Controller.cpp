#include "Animator_Controller.h"
#include "Animator_State_Machine.h"
#include "Animator_State_Transition.h"
#include "Transform.h"
#include "Time_Engine.h"
#include "System_Function.h"
#include "Include_ImGui.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include <locale.h>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;
using namespace BeastEngine;

void Animator_Controller::Initialize(shared_ptr<Transform>& root)
{
	Engine::asset_manager->Registration_Asset(shared_from_this());
	root_transform = root;
	if (!parameters)
	{
		parameters = make_shared<unordered_map<string, Controller_Parameter>>();
	}
	for (auto& state : state_machines)
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

bool Animator_Controller::Add_State_Machine(string& name)
{
	bool already = false;
	for (auto& state : state_machines)
	{
		if (state->name == name) { already = true; return false; }
	}

	shared_ptr<Animator_State_Machine> state_machine = make_shared<Animator_State_Machine>();
	state_machine->name = name;
	state_machine->Initialize(parameters);
	state_machines.push_back(state_machine);

	return true;
}

bool Animator_Controller::Remove_State_Machine(string& name)
{
	shared_ptr<Animator_State_Machine> target_state;
	size_t state_size = state_machines.size();

	for (size_t i = 0; i < state_size; ++i)
	{
		if (state_machines[i]->name == name)
		{
			target_state = state_machines[i];
			state_machines.erase(state_machines.begin() + i);
			break;
		}
	}

	if (!target_state) return false;

	state_size = state_machines.size();
	for (size_t i = 0; i < state_size; ++i)
	{
		auto it = state_machines[i]->transitions.begin();
		while (it != state_machines[i]->transitions.end())
		{
			if ((*it)->next_state.lock() == target_state)
			{
				it = state_machines[i]->transitions.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	return true;
}

void Animator_Controller::Set_Animation_Target(shared_ptr<Animator_State_Machine>& state)
{
	if (state->clip)
	{
		shared_ptr<Animation_Clip>& clip = state->clip;
		pose_default.clear();
		pose_playing.clear();
		pose_next.clear();
		pose_interrupt.clear();

		for (auto& anim : clip->animations)
		{
			if (clip->is_humanoid)
			{
				//TODO:Avatar���m�肵������
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
						Animation_Target t = { Humanoid_Rig::None,t_trans, t_trans->Get_Local_Position(),t_trans->Get_Local_Rotation(),t_trans->Get_Local_Scale() };
						pose_default[anim.Target_Path] = t;
					}
				}
			}
		}
		pose_playing = pose_default;
		pose_next = pose_default;
		pose_interrupt.clear();
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
			pose_interrupt.clear();
			active_transition = playing_state_machine->Get_Active_Transition();
			next_state_machine = active_transition->next_state.lock();
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
					next_state_machine = active_transition->next_state.lock();
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
					next_state_machine = active_transition->next_state.lock();
					next_state_machine->Set_Active(active_transition->transition_offset);
					next_state_machine->transition_trigger = false;
				}
			}
		}
	}

	pose_playing = pose_default;
	pose_next = pose_default;
	animation_data = pose_default;

	if (next_state_machine)
	{
		const float& current_sec = playing_state_machine->currentSeconds;
		for (auto& animation : playing_state_machine->clip->animations)
		{
			const vector<Animation_Clip::Keyframe>& keyframes = animation.keys;

			const int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
				const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

				if (current_sec >= keyframe0.time && current_sec < keyframe1.time)
				{
					const float rate = (current_sec - keyframe0.time) / (keyframe1.time - keyframe0.time);

					Animation_Target& target = pose_playing[animation.Target_Path];
					// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
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

		const float& next_sec = next_state_machine->currentSeconds;
		for (auto& animation : next_state_machine->clip->animations)
		{
			const vector<Animation_Clip::Keyframe>& keyframes = animation.keys;

			int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
				const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

				if (next_sec >= keyframe0.time && next_sec < keyframe1.time)
				{
					const float rate = (next_sec - keyframe0.time) / (keyframe1.time - keyframe0.time);

					Animation_Target& target = pose_next[animation.Target_Path];
					// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
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

		if (!playing_state_machine->clip->is_humanoid)
		{
			const float rate = duration_timer / active_transition->transition_duration;
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
	}
	else
	{
		const float& current_sec = playing_state_machine->currentSeconds;
		for (auto& animation : playing_state_machine->clip->animations)
		{
			const vector<Animation_Clip::Keyframe>& keyframes = animation.keys;

			const int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
				const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

				if (current_sec >= keyframe0.time && current_sec < keyframe1.time)
				{
					const float rate = (current_sec - keyframe0.time) / (keyframe1.time - keyframe0.time);

					Animation_Target& target = pose_playing[animation.Target_Path];
					// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
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

void Animator_Controller::Save()
{
	string path = System_Function::Get_Save_File_Name();
	if (path != "")
	{
		int path_i = path.find_last_of("\\") + 1;//7
		int ext_i = path.find_last_of(".");//10
		string pathname = path.substr(0, path_i); //�t�@�C���܂ł̃f�B���N�g��
		string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
		path = pathname + filename + ".controller";

		name = filename;
		ofstream ss(path.c_str(), ios::binary);
		{
			cereal::BinaryOutputArchive o_archive(ss);
			o_archive(static_pointer_cast<Animator_Controller>(shared_from_this()));
		}
		save_path = path;
	}
}

void Animator_Controller::Save_As()
{
	if (save_path.empty())
	{
		Save();
	}
	else
	{
		ofstream ss(save_path.c_str(), ios::binary);
		{
			cereal::BinaryOutputArchive o_archive(ss);
			o_archive(static_pointer_cast<Animator_Controller>(shared_from_this()));
		}
	}
}

shared_ptr<Animator_Controller> Animator_Controller::Load_Animator_Controller(string fullpath)
{
	string path;
	if (fullpath.empty())
	{
		path = System_Function::Get_Open_File_Name("controller", "\\Resouces\\Model");
	}
	else
	{
		path = fullpath;
	}
	int path_i = path.find_last_of("\\") + 1;//7
	int ext_i = path.find_last_of(".");//10
	string pathname = path.substr(0, path_i); //�t�@�C���܂ł̃f�B���N�g��
	string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
	shared_ptr<Animator_Controller> controller = make_shared<Animator_Controller>();

	ifstream in_bin(path, ios::binary);
	if (in_bin.is_open())
	{
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
		binaryInputArchive(controller);
		return controller;
	}

	return nullptr;
}

shared_ptr<Animator_Controller> Animator_Controller::Create_New_Controller()
{
	string path = System_Function::Get_Save_File_Name("controller", "\\Resouces\\Model");
	if (path != "")
	{
		int path_i = path.find_last_of("\\") + 1;//7
		int ext_i = path.find_last_of(".");//10
		string pathname = path.substr(0, path_i); //�t�@�C���܂ł̃f�B���N�g��
		string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
		path = pathname + filename + ".controller";

		shared_ptr<Animator_Controller> controller = make_shared<Animator_Controller>();
		controller->name = filename;
		controller->save_path = path;
		ofstream ss(path.c_str(), ios::binary);
		{
			cereal::BinaryOutputArchive o_archive(ss);
			o_archive(controller);
		}
		return controller;
	}
	return nullptr;
}

void Animator_Controller::Add_Parameter(string& p_name, Condition_Type type)
{
	auto it = parameters->find(p_name);
	if (it == parameters->end())
	{
		Controller_Parameter parameter = {};
		parameter.type = type;
		parameters->insert(make_pair(p_name, parameter));
	}
}

void Animator_Controller::Render_ImGui()
{
	string controller_name = name + ".controller";
	ImGui::Text(controller_name.c_str());
	ImGui::SameLine();
	if (ImGui::Button(u8"�ʖ��ŕۑ�"))
	{
		Save();
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"�㏑���ۑ�"))
	{
		Save_As();
	}
	static bool Auto_Save = true;
	ImGui::SameLine();
	ImGui::Checkbox(u8"�I�[�g�Z�[�u", &Auto_Save);

	ImGui::Dummy(ImVec2(0, 10.0f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, 10.0f));
	{
		ImGui::BeginChild("Parameters", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.25f, 0), true, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu(u8"�p�^���[�^�ǉ�"))
			{
				static string parameter_name;
				ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
				if (ImGui::BeginMenu("Int"))
				{
					ImGui::InputText(u8"�p�����[�^��", &parameter_name, input_text_flags);
					if (ImGui::Button(u8"�ǉ�") && !parameter_name.empty())
					{
						Add_Parameter(parameter_name, Condition_Type::Int);
						parameter_name.clear();
						if (Auto_Save) Save_As();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Float"))
				{
					ImGui::InputText(u8"�p�����[�^��", &parameter_name, input_text_flags);
					if (ImGui::Button(u8"�ǉ�") && !parameter_name.empty())
					{
						Add_Parameter(parameter_name, Condition_Type::Float);
						parameter_name.clear();
						if (Auto_Save) Save_As();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Bool"))
				{
					ImGui::InputText(u8"�p�����[�^��", &parameter_name, input_text_flags);
					if (ImGui::Button(u8"�ǉ�") && !parameter_name.empty())
					{
						Add_Parameter(parameter_name, Condition_Type::Bool);
						parameter_name.clear();
						if (Auto_Save) Save_As();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Trigger"))
				{
					ImGui::InputText(u8"�p�����[�^��", &parameter_name, input_text_flags);
					if (ImGui::Button(u8"�ǉ�") && !parameter_name.empty())
					{
						Add_Parameter(parameter_name, Condition_Type::Trigger);
						parameter_name.clear();
						if (Auto_Save) Save_As();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		bool erase_parameter = false;
		string erase_key;
		for (auto& parameter : (*parameters))
		{
			ImGui::Spacing();
			ImGui::Text(parameter.first.c_str());
			ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.6f);
			ImGui::PushID(parameter.first.c_str());
			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionMax().x * 0.3f);
			switch (parameter.second.type)
			{
				case Condition_Type::Int:
					if (ImGui::InputInt("##int", &parameter.second.value_int))
					{
						if (Auto_Save) Save_As();
					}
					break;
				case Condition_Type::Float:
					if (ImGui::InputFloat("##float", &parameter.second.value_float))
					{
						if (Auto_Save) Save_As();
					}
					break;
				case Condition_Type::Bool:
					if (ImGui::Checkbox("##bool", &parameter.second.value_bool))
					{
						if (Auto_Save) Save_As();
					}
					break;
				case Condition_Type::Trigger:
					if (ImGui::Checkbox("##trigger", &parameter.second.value_bool))
					{
						if (Auto_Save) Save_As();
					}
					break;
			}

			ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.0f, 0.0f, 1.0f });
			if (ImGui::Button(u8" �~ "))
			{
				erase_parameter = true;
				erase_key = parameter.first;
			}
			ImGui::PopStyleColor(1);
			ImGui::PopID();
		}
		if (erase_parameter)
		{
			parameters->erase(erase_key);
			if (Auto_Save) Save_As();
		}
		ImGui::EndChild();
	}

	ImGui::SameLine();

	{
		ImGui::BeginChild("States", ImVec2(0, 0));

		const float input_padding = ImGui::GetWindowContentRegionMax().x * 0.4f;
		const float input_size = ImGui::GetWindowContentRegionMax().x - input_padding;

		shared_ptr<Animator_State_Machine> current_state;
		bool has_state = !state_machines.empty();

		if (has_state)
		{
			current_state = state_machines[current_state_index];
			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionMax().x * 0.8f);
			if (ImGui::BeginCombo("##State_Select", current_state->name.data()))
			{
				const size_t state_size = state_machines.size();
				for (size_t i = 0; i < state_size; ++i)
				{
					const bool is_selected = (current_state_index == i);
					if (ImGui::Selectable(state_machines[i]->name.data(), is_selected))
					{
						current_state_index = i;
					}

					if (is_selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		else
		{
			ImGui::Text(u8"�A�j���[�V�����X�e�[�g������܂���");
			current_state_index = 0;
		}

		ImGui::SameLine();
		if (ImGui::Button(u8"�ǉ�##�X�e�[�g"))
		{
			ImGui::OpenPopup(u8"�V�K�X�e�[�g�쐬���j���[");
		}
		if (ImGui::BeginPopup(u8"�V�K�X�e�[�g�쐬���j���["))
		{
			static string state_name;
			ImGui::InputText(u8"�V�K�X�e�[�g��", &state_name, ImGuiInputTextFlags_EnterReturnsTrue);
			if (ImGui::Button(u8"�o�^") && !state_name.empty())
			{
				Add_State_Machine(state_name);
				state_name.clear();
				current_state_index = static_cast<int>(state_machines.size()) - 1;
				current_state = state_machines[current_state_index];
				if (Auto_Save) Save_As();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (has_state)
		{
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.0f, 0.0f, 1.0f });
			if (ImGui::Button(u8"�폜##�X�e�[�g"))
			{
				ImGui::OpenPopup(u8"�X�e�[�g�폜�m�F");
			}
			ImGui::PopStyleColor(1);

			if (ImGui::BeginPopupModal(u8"�X�e�[�g�폜�m�F", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text(u8"�X�e�[�g���폜���܂����H\n���̑���͎������܂���\n\n");
				ImGui::Separator();

				if (ImGui::Button(u8"�폜����", ImVec2(150, 0)))
				{
					Remove_State_Machine(current_state->name);
					current_state_index = 0;
					if (state_machines.size() >= 1)
					{
						current_state = state_machines[current_state_index];
					}
					else
					{
						current_state.reset();
						has_state = false;
					}
					if (Auto_Save) Save_As();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button(u8"�L�����Z��", ImVec2(150, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}
		}

		if (has_state)
		{
			ImGui::Spacing();
			ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			if (ImGui::CollapsingHeader(u8"�X�e�[�g�ݒ�", ImGuiTreeNodeFlags_AllowItemOverlap))
			{
				ImGui::Indent();
				ImGui::Text(u8"�N���b�v");
				ImGui::SameLine(input_padding);
				string clip_name;
				if (current_state->clip)
				{
					clip_name = current_state->clip->name;
				}
				else
				{
					clip_name = u8"�N���b�v���I������Ă��܂���";
				}
				if (ImGui::Button(clip_name.c_str(), ImVec2(200, 0)))
				{
					string path = System_Function::Get_Open_File_Name("anim", "\\Resouces\\Model");
					if (!path.empty())
					{
						current_state->Set_Clip(path);
						Set_Animation_Target(current_state);
						if (Auto_Save) Save_As();
					}
				}

				ImGui::Text(u8"�Đ����x");
				ImGui::SameLine(input_padding);
				ImGui::SetNextItemWidth(input_size);
				if (ImGui::InputFloat("##Speed", &current_state->animation_speed))
				{
					if (Auto_Save) Save_As();
				}

				ImGui::Indent();
				ImGui::Text(u8"�搔");
				ImGui::SameLine(input_padding);
				ImGui::SetNextItemWidth(input_size);
				ImGui::InputText("##Multiply", &current_state->multiplier_hash);
				ImGui::Unindent();

				ImGui::Text(u8"���[�v�A�j���[�V����");
				ImGui::SameLine(input_padding);
				if (ImGui::Checkbox("##Loop", &current_state->loopAnimation))
				{
					if (Auto_Save) Save_As();
				}

				ImGui::Text(u8"�f�t�H���g�X�e�[�g");
				ImGui::SameLine(input_padding);
				if (ImGui::Checkbox("##Default", &current_state->is_default_state))
				{
					if (Auto_Save) Save_As();
				}

				ImGui::Dummy(ImVec2(0, 20.0f));
				ImGui::Unindent();
			}

			ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			if (ImGui::CollapsingHeader(u8"�J�ڐݒ�", ImGuiTreeNodeFlags_AllowItemOverlap))
			{
				ImGui::Indent();
				static int current_transition_index = 0;
				shared_ptr<Animator_State_Transition> current_transition;
				bool has_transition = !current_state->transitions.empty();

				if (has_transition)
				{
					current_transition = current_state->transitions[current_transition_index];
					if (ImGui::BeginCombo("##Transition_Select", current_transition->next_state.lock()->name.data()))
					{
						const size_t transitions_size = current_state->transitions.size();
						for (size_t n = 0; n < transitions_size; ++n)
						{
							ImGui::PushID(to_string(n).c_str());
							const bool is_selected = (current_transition_index == n);
							if (ImGui::Selectable(current_transition->next_state.lock()->name.data(), is_selected))
							{
								current_transition_index = n;
							}

							if (is_selected) ImGui::SetItemDefaultFocus();

							ImGui::PopID();
						}
						ImGui::EndCombo();
					}
				}
				else
				{
					ImGui::Text(u8"�J�ڐݒ肪����܂���");
					current_transition_index = 0;
				}

				if (state_machines.size() >= 2)
				{
					ImGui::SameLine();
					if (ImGui::Button(u8"�ǉ�##�J��"))
					{
						ImGui::OpenPopup(u8"�V�K�J�ڍ쐬���j���[");
					}
					if (ImGui::BeginPopup(u8"�V�K�J�ڍ쐬���j���["))
					{
						static int next_state_index = 0;
						if (next_state_index == current_state_index)
						{
							if (current_state_index == 0)
							{
								next_state_index = 1;
							}
							else
							{
								next_state_index = 0;
							}
						}
						if (ImGui::BeginCombo("##Next_Select", state_machines[next_state_index]->name.data()))
						{
							static size_t state_size;
							state_size = state_machines.size();
							for (size_t i = 0; i < state_size; ++i)
							{
								if (i != current_state_index)
								{
									const bool is_selected = (next_state_index == i);
									if (ImGui::Selectable(state_machines[i]->name.data(), is_selected))
									{
										next_state_index = i;
									}
									if (is_selected) ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}
						if (ImGui::Button(u8"�o�^") && current_state_index != next_state_index)
						{
							current_state->Add_Transition(state_machines[next_state_index]);
							next_state_index = 0;
							if (Auto_Save) Save_As();
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}

					if (has_transition)
					{
						ImGui::SameLine();
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.0f, 0.0f, 1.0f });
						if (ImGui::Button(u8"�폜##�J��"))
						{
							ImGui::OpenPopup(u8"�J�ڍ폜�m�F");
						}
						ImGui::PopStyleColor(1);

						if (ImGui::BeginPopupModal(u8"�J�ڍ폜�m�F", NULL, ImGuiWindowFlags_AlwaysAutoResize))
						{
							ImGui::Text(u8"�J�ڂ��폜���܂����H\n���̑���͎������܂���\n\n");
							ImGui::Separator();

							if (ImGui::Button(u8"�폜����", ImVec2(150, 0)))
							{
								current_state->Remove_Transition(current_transition_index);
								current_transition_index = 0;
								if (current_state->transitions.size() >= 1)
								{
									current_transition = current_state->transitions[current_transition_index];
								}
								else
								{
									current_transition.reset();
									has_transition = false;
								}
								if (Auto_Save) Save_As();
								ImGui::CloseCurrentPopup();
							}
							ImGui::SameLine();
							if (ImGui::Button(u8"�L�����Z��", ImVec2(150, 0))) { ImGui::CloseCurrentPopup(); }
							ImGui::EndPopup();
						}
					}

					if (has_transition)
					{
						ImGui::Text(u8"���莞�Ԃ���");
						ImGui::SameLine(input_padding);
						if (ImGui::Checkbox("##exit", &current_transition->has_exit_time))
						{
							if (Auto_Save) Save_As();
						}

						ImGui::Indent();
						ImGui::Text(u8"���莞��");
						ImGui::SameLine(input_padding);
						ImGui::SetNextItemWidth(input_size);
						if (ImGui::InputFloat("##exit_time", &current_transition->exit_time))
						{
							if (Auto_Save) Save_As();
						}
						ImGui::Unindent();

						ImGui::Text(u8"�J�ڊԊu(�b)");
						ImGui::SameLine(input_padding);
						ImGui::SetNextItemWidth(input_size);
						if (ImGui::InputFloat("##Duration", &current_transition->transition_duration))
						{
							if (Auto_Save) Save_As();
						}

						ImGui::Text(u8"�J�ڃI�t�Z�b�g");
						ImGui::SameLine(input_padding);
						ImGui::SetNextItemWidth(input_size);
						if (ImGui::InputFloat("##offset", &current_transition->transition_offset))
						{
							if (Auto_Save) Save_As();
						}

						ImGui::Text(u8"���f�v��");
						ImGui::SameLine(input_padding);
						ImGui::SetNextItemWidth(input_size);
						const char* sources[3] = { "None", "Current_State", "Next_State" };
						int current_sources = static_cast<int>(current_transition->interruption_source);
						ImGui::Combo("##interruption_source", &current_sources, sources, IM_ARRAYSIZE(sources));
						if (current_sources != static_cast<int>(current_transition->interruption_source))
						{
							current_transition->interruption_source = static_cast<Animator_State_Transition::Interruption_Source>(current_sources);
							if (Auto_Save) Save_As();
						}

						ImGui::Spacing();
						ImGui::Indent();
						ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
						if (ImGui::CollapsingHeader(u8"�J�ڏ���"))
						{
							const float window_width = ImGui::GetWindowContentRegionMax().x;
							const size_t condition_size = current_transition->conditions.size();
							if (condition_size >= 1)
							{
								bool erase_condition = false;
								int erase_condition_index;
								for (size_t i = 0; i < condition_size; ++i)
								{
									shared_ptr<Condition>& condition = current_transition->conditions[i];
									ImGui::PushID(to_string(i).c_str());
									ImGui::SetNextItemWidth(window_width * 0.3f);
									if (ImGui::BeginCombo("##Transition_Select", condition->key.data()))
									{
										for (auto& parameter : (*parameters))
										{
											const bool is_selected = (parameter.first == condition->key);
											if (ImGui::Selectable(parameter.first.data(), is_selected))
											{
												condition->key = parameter.first;
												condition->type = parameter.second.type;
												condition->threshold = 0;
												if (condition->type == Condition_Type::Int || condition->type == Condition_Type::Float)
												{
													condition->mode = Condition_Mode::Greater;
												}
												else if (condition->type == Condition_Type::Bool || condition->type == Condition_Type::Trigger)
												{
													condition->mode = Condition_Mode::If;
												}
												if (Auto_Save) Save_As();
											}

											if (is_selected)
											{
												ImGui::SetItemDefaultFocus();
											}
										}
										ImGui::EndCombo();
									}

									auto it = parameters->find(condition->key);
									if (it != parameters->end())
									{
										if (it->second.type != Condition_Type::Trigger)
										{
											const char* mode_char[6] = { "True","False","Greater","Less","Equals","NotEquals" };
											ImGui::SameLine();
											ImGui::SetNextItemWidth(window_width * 0.2f);
											const int current_mode = static_cast<int>(condition->mode);
											if (ImGui::BeginCombo("##mode", mode_char[current_mode]))
											{
												int mode_start, mode_end = 0;
												if (it->second.type == Condition_Type::Int) { mode_start = 2; mode_end = 6; }
												else if (it->second.type == Condition_Type::Float) { mode_start = 2; mode_end = 4; }
												else if (it->second.type == Condition_Type::Bool) { mode_start = 0; mode_end = 2; }

												for (; mode_start < mode_end; ++mode_start)
												{
													const bool is_selected = (mode_start == current_mode);
													if (ImGui::Selectable(mode_char[mode_start], is_selected))
													{
														condition->mode = static_cast<Condition_Mode>(mode_start);
														if (Auto_Save) Save_As();
													}
													if (is_selected) ImGui::SetItemDefaultFocus();
												}
												ImGui::EndCombo();
											}

											if (it->second.type == Condition_Type::Int)
											{
												ImGui::SameLine();
												ImGui::SetNextItemWidth(window_width * 0.2f);
												int threshold = static_cast<int>(condition->threshold);
												if (ImGui::InputInt("##Input_Int", &threshold))
												{
													condition->threshold = static_cast<float>(threshold);
													if (Auto_Save) Save_As();
												}
											}
											else if (it->second.type == Condition_Type::Float)
											{
												ImGui::SameLine();
												ImGui::SetNextItemWidth(window_width * 0.2f);
												if (ImGui::InputFloat("##Input_Float", &condition->threshold))
												{
													if (Auto_Save) Save_As();
												}
											}
										}
									}

									ImGui::SameLine(window_width - 20.0f);
									ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.0f, 0.0f, 1.0f });
									if (ImGui::Button(u8" �~ "))
									{
										erase_condition = true;
										erase_condition_index = i;
									}
									ImGui::PopStyleColor(1);
									ImGui::PopID();
								}

								if (erase_condition)
								{
									current_transition->conditions.erase(current_transition->conditions.begin() + erase_condition_index);
									if (Auto_Save) Save_As();
								}
							}
							else
							{
								ImGui::Text(u8"����������܂���");
							}

							ImGui::Dummy(ImVec2(0, 0));
							ImGui::SameLine(window_width - 100.0f);
							if (ImGui::Button(u8"�����ǉ�", ImVec2(100.0f, 0)))
							{
								if (!parameters->empty())
								{
									auto it = parameters->begin();
									Condition_Mode mode = Condition_Mode::Greater;
									if (it->second.type == Condition_Type::Bool || it->second.type == Condition_Type::Trigger)
									{
										mode = Condition_Mode::If;
									}
									current_transition->Add_Condition(it->first, it->second.type, mode, 0.0f);
									if (Auto_Save) Save_As();
								}
							}
						}
						ImGui::Unindent();
					}
				}
				ImGui::Unindent();
			}
		}

		ImGui::EndChild();
	}
}