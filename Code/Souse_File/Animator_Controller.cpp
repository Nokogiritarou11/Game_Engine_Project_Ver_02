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
	shared_ptr<Animation_Clip>& clip = state->clip;

	if (!clip->is_humanoid)
	{
		for (auto& anim : clip->animations)
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

	if (next_state_machine)
	{
		for (auto& animation : playing_state_machine->clip->animations)
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
		for (auto& animation : next_state_machine->clip->animations)
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
		for (auto& animation : playing_state_machine->clip->animations)
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

void Animator_Controller::Save()
{
	string path = System_Function::Get_Save_File_Name();
	if (path != "")
	{
		int path_i = path.find_last_of("\\") + 1;//7
		int ext_i = path.find_last_of(".");//10
		string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
		string filename = path.substr(path_i, ext_i - path_i); //ファイル名
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
	string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
	string filename = path.substr(path_i, ext_i - path_i); //ファイル名
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
		string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
		string filename = path.substr(path_i, ext_i - path_i); //ファイル名
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
	static string controller_name;
	controller_name = name + ".controller";
	ImGui::Text(controller_name.c_str());
	ImGui::SameLine();
	if (ImGui::Button(u8"別名で保存"))
	{
		Save();
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"上書き保存"))
	{
		Save_As();
	}

	{
		ImGui::BeginChild("Parameters", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, 0), true, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu(u8"追加"))
			{
				static string parameter_name;
				ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
				if (ImGui::BeginMenu("Int"))
				{
					ImGui::InputText(u8"パラメータ名", &parameter_name, input_text_flags);
					if (ImGui::Button(u8"追加") && !parameter_name.empty())
					{
						Add_Parameter(parameter_name, Condition_Type::Int);
						parameter_name.clear();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Float"))
				{
					ImGui::InputText(u8"パラメータ名", &parameter_name, input_text_flags);
					if (ImGui::Button(u8"追加") && !parameter_name.empty())
					{
						Add_Parameter(parameter_name, Condition_Type::Float);
						parameter_name.clear();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Bool"))
				{
					ImGui::InputText(u8"パラメータ名", &parameter_name, input_text_flags);
					if (ImGui::Button(u8"追加") && !parameter_name.empty())
					{
						Add_Parameter(parameter_name, Condition_Type::Bool);
						parameter_name.clear();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Trigger"))
				{
					ImGui::InputText(u8"パラメータ名", &parameter_name, input_text_flags);
					if (ImGui::Button(u8"追加") && !parameter_name.empty())
					{
						Add_Parameter(parameter_name, Condition_Type::Trigger);
						parameter_name.clear();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		static bool erase_parameter;
		static string erase_key;
		for (auto& parameter : (*parameters))
		{
			ImGui::Spacing();
			ImGui::Text(parameter.first.c_str());
			ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.6f);
			ImGui::PushID(parameter.first.c_str());
			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionMax().x * 0.25f);
			switch (parameter.second.type)
			{
				case Condition_Type::Int:
					ImGui::InputInt("##int", &parameter.second.value_int);
					break;
				case Condition_Type::Float:
					ImGui::InputFloat("##float", &parameter.second.value_float);
					break;
				case Condition_Type::Bool:
					ImGui::Checkbox("##bool", &parameter.second.value_bool);
					break;
				case Condition_Type::Trigger:
					ImGui::Checkbox("##trigger", &parameter.second.value_bool);
					break;
			}

			ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.0f, 0.0f, 1.0f });
			if (ImGui::Button(u8" × "))
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
			erase_parameter = false;
			erase_key.clear();
		}
		ImGui::EndChild();
	}

	ImGui::SameLine();

	{
		ImGui::BeginChild("States", ImVec2(0, 0));

		shared_ptr<Animator_State_Machine> current_state;
		static bool has_state;
		has_state = !state_machines.empty();

		if (has_state)
		{
			current_state = state_machines[current_state_index];
			if (ImGui::BeginCombo("##State_Select", current_state->name.data()))
			{
				static size_t state_size;
				state_size = state_machines.size();
				for (size_t i = 0; i < state_size; ++i)
				{
					const bool is_selected = (current_state_index == i);
					if (ImGui::Selectable(state_machines[i]->name.data(), is_selected))
					{
						current_state_index = i;
					}

					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}
		else
		{
			ImGui::Text(u8"アニメーションステートがありません");
			current_state_index = 0;
		}

		ImGui::SameLine();
		if (ImGui::Button(u8"新規アニメーションステートを追加"))
		{
			ImGui::OpenPopup(u8"新規ステート作成メニュー");
		}
		if (ImGui::BeginPopup(u8"新規ステート作成メニュー"))
		{
			static string state_name;
			ImGui::InputText(u8"新規ステート名", &state_name, ImGuiInputTextFlags_EnterReturnsTrue);
			if (ImGui::Button(u8"登録") && !state_name.empty())
			{
				Add_State_Machine(state_name);
				state_name.clear();
				current_state_index = static_cast<int>(state_machines.size()) - 1;
				current_state = state_machines[current_state_index];
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (has_state)
		{
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.0f, 0.0f, 1.0f });
			if (ImGui::Button(u8"ステートを削除"))
			{
				ImGui::OpenPopup(u8"ステート削除確認");
			}
			ImGui::PopStyleColor(1);

			if (ImGui::BeginPopupModal(u8"ステート削除確認", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text(u8"ステートを削除しますか？\nこの操作は取り消せません\n\n");
				ImGui::Separator();

				if (ImGui::Button(u8"削除する", ImVec2(150, 0)))
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
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button(u8"キャンセル", ImVec2(150, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}
		}

		if (has_state)
		{
			ImGui::Spacing();
			if (ImGui::CollapsingHeader(u8"ステート設定", ImGuiTreeNodeFlags_AllowItemOverlap))
			{
				ImGui::Text(u8"クリップ");
				ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.4f);
				if (current_state->clip)
				{
					ImGui::Text(current_state->clip->name.c_str());
				}
				else
				{
					ImGui::Text(u8"クリップが選択されていません");
				}
				ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 30.0f);
				if (ImGui::Button(u8"選択"))
				{
					string path = System_Function::Get_Open_File_Name("anim", "\\Resouces\\Model");
					if (!path.empty())
					{
						current_state->Set_Clip(path);
						Set_Animation_Target(current_state);
					}
				}

				ImGui::Text(u8"再生速度");
				ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.4f);
				ImGui::InputFloat("##Speed", &current_state->animation_speed);

				ImGui::Indent();
				ImGui::Text(u8"乗数");
				ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.4f);
				ImGui::InputText("##Multiply", &current_state->multiplier_hash);
				ImGui::Unindent();

				ImGui::Text(u8"ループアニメーション");
				ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.4f);
				ImGui::Checkbox("##Loop", &current_state->loopAnimation);

				ImGui::Text(u8"デフォルトステート");
				ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.4f);
				ImGui::Checkbox("##Default", &current_state->is_default_state);
			}

			ImGui::Spacing();

			if (ImGui::CollapsingHeader(u8"遷移設定", ImGuiTreeNodeFlags_AllowItemOverlap))
			{
				static int current_transition_index = 0;
				shared_ptr<Animator_State_Transition> current_transition;
				static bool has_transition;
				has_transition = !current_state->transitions.empty();

				if (has_transition)
				{
					current_transition = current_state->transitions[current_transition_index];
					if (ImGui::BeginCombo("##Transition_Select", current_transition->next_state.lock()->name.data()))
					{
						static size_t transitions_size;
						transitions_size = current_state->transitions.size();
						for (size_t n = 0; n < transitions_size; ++n)
						{
							ImGui::PushID(to_string(n).c_str());
							const bool is_selected = (current_transition_index == n);
							if (ImGui::Selectable(current_transition->next_state.lock()->name.data(), is_selected))
							{
								current_transition_index = n;
							}

							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
							ImGui::PopID();
						}
						ImGui::EndCombo();
					}
				}
				else
				{
					ImGui::Text(u8"遷移設定がありません");
					current_transition_index = 0;
				}

				if (state_machines.size() >= 2)
				{
					ImGui::SameLine();
					if (ImGui::Button(u8"新規遷移設定を追加"))
					{
						ImGui::OpenPopup(u8"新規遷移作成メニュー");
					}
					if (ImGui::BeginPopup(u8"新規遷移作成メニュー"))
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

									if (is_selected)
									{
										ImGui::SetItemDefaultFocus();
									}
								}
							}
							ImGui::EndCombo();
						}
						if (ImGui::Button(u8"登録") && current_state_index != next_state_index)
						{
							current_state->Add_Transition(state_machines[next_state_index]);
							next_state_index = 0;
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}

					if (has_transition)
					{
						ImGui::SameLine();
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.0f, 0.0f, 1.0f });
						if (ImGui::Button(u8"遷移を削除"))
						{
							ImGui::OpenPopup(u8"遷移削除確認");
						}
						ImGui::PopStyleColor(1);

						if (ImGui::BeginPopupModal(u8"遷移削除確認", NULL, ImGuiWindowFlags_AlwaysAutoResize))
						{
							ImGui::Text(u8"遷移を削除しますか？\nこの操作は取り消せません\n\n");
							ImGui::Separator();

							if (ImGui::Button(u8"削除する", ImVec2(150, 0)))
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
								ImGui::CloseCurrentPopup();
							}
							ImGui::SameLine();
							if (ImGui::Button(u8"キャンセル", ImVec2(150, 0))) { ImGui::CloseCurrentPopup(); }
							ImGui::EndPopup();
						}
					}

					if (has_transition)
					{
						ImGui::Text(u8"判定時間あり");
						ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.4f);
						ImGui::Checkbox("##exit", &current_transition->has_exit_time);

						ImGui::Indent();
						ImGui::Text(u8"判定時間");
						ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.4f);
						ImGui::InputFloat("##exit_time", &current_transition->exit_time);
						ImGui::Unindent();

						ImGui::Text(u8"遷移間隔(秒)");
						ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.4f);
						ImGui::InputFloat("##Duration", &current_transition->transition_duration);

						ImGui::Text(u8"遷移オフセット");
						ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.4f);
						ImGui::InputFloat("##offset", &current_transition->transition_offset);

						ImGui::Text(u8"中断要因");
						ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.4f);
						const static char* sources[3] = { "None", "Current_State", "Next_State" };
						static int current_sources = 0;
						current_sources = static_cast<int>(current_transition->interruption_source);
						ImGui::Combo("##interruption_source", &current_sources, sources, IM_ARRAYSIZE(sources));
						if (current_sources != static_cast<int>(current_transition->interruption_source))
						{
							current_transition->interruption_source = static_cast<Animator_State_Transition::Interruption_Source>(current_sources);
						}
					}
				}
			}
		}

		ImGui::EndChild();
	}
}