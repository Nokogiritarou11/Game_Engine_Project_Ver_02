#include "Animator_Controller.h"
#include "Animator_State_Machine.h"
#include "Animator_State_Transition.h"
#include "Transform.h"
#include "Time_Engine.h"
#include "System_Function.h"
#include "Include_ImGui.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include "Debug.h"
#include <locale.h>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;
using namespace BeastEngine;

void Animator_Controller::Initialize()
{
	if (!parameters)
	{
		parameters = make_shared<unordered_map<string, Animation_Parameter>>();
	}
	for (auto& state : state_machines)
	{
		state->Initialize(parameters);
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
	state_machines.emplace_back(state_machine);

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

void Animator_Controller::Update()
{
	if (!active_transition)
	{
		playing_state_machine->Update_Transition();
		playing_state_machine->Update_Time();

		if (playing_state_machine->transition_trigger)
		{
			duration_timer = 0;
			interrupt_state = 1;
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
			interrupt_state = 1;
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
					interrupt_state = 2;
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
					interrupt_state = 2;
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
	shared_ptr<Animator_Controller> controller;

	ifstream in_bin(path, ios::binary);
	if (in_bin.is_open())
	{
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
		binaryInputArchive(controller);
		Engine::asset_manager->Registration_Asset(controller);
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
		Engine::asset_manager->Registration_Asset(controller);
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

void Animator_Controller::Add_Parameter(string& p_name, Parameter_Type type)
{
	auto it = parameters->find(p_name);
	if (it == parameters->end())
	{
		Animation_Parameter parameter = {};
		parameter.type = type;
		parameters->insert(make_pair(p_name, parameter));
	}
}

void Animator_Controller::Render_ImGui()
{
	string controller_name = name + ".controller";
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
	static bool Auto_Save = true;
	ImGui::SameLine();
	ImGui::Checkbox(u8"オートセーブ", &Auto_Save);

	ImGui::Dummy({ 0, 10.0f });
	ImGui::Separator();
	ImGui::Dummy({ 0, 10.0f });
	{
		ImGui::BeginChild("Parameters", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.25f, 0), true, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu(u8"パタメータ追加"))
			{
				static string parameter_name;
				ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
				if (ImGui::BeginMenu("Int"))
				{
					ImGui::InputText(u8"パラメータ名", &parameter_name, input_text_flags);
					if (ImGui::Button(u8"追加") && !parameter_name.empty())
					{
						Add_Parameter(parameter_name, Parameter_Type::Int);
						parameter_name.clear();
						if (Auto_Save) Save_As();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Float"))
				{
					ImGui::InputText(u8"パラメータ名", &parameter_name, input_text_flags);
					if (ImGui::Button(u8"追加") && !parameter_name.empty())
					{
						Add_Parameter(parameter_name, Parameter_Type::Float);
						parameter_name.clear();
						if (Auto_Save) Save_As();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Bool"))
				{
					ImGui::InputText(u8"パラメータ名", &parameter_name, input_text_flags);
					if (ImGui::Button(u8"追加") && !parameter_name.empty())
					{
						Add_Parameter(parameter_name, Parameter_Type::Bool);
						parameter_name.clear();
						if (Auto_Save) Save_As();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Trigger"))
				{
					ImGui::InputText(u8"パラメータ名", &parameter_name, input_text_flags);
					if (ImGui::Button(u8"追加") && !parameter_name.empty())
					{
						Add_Parameter(parameter_name, Parameter_Type::Trigger);
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
			ImGui::SameLine(ImGui::GetWindowContentRegionWidth() * 0.6f);
			ImGui::PushID(parameter.first.c_str());
			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
			switch (parameter.second.type)
			{
				case Parameter_Type::Int:
					if (ImGui::InputInt("##int", &parameter.second.value_int))
					{
						if (Auto_Save) Save_As();
					}
					break;
				case Parameter_Type::Float:
					if (ImGui::InputFloat("##float", &parameter.second.value_float))
					{
						if (Auto_Save) Save_As();
					}
					break;
				case Parameter_Type::Bool:
					if (ImGui::Checkbox("##bool", &parameter.second.value_bool))
					{
						if (Auto_Save) Save_As();
					}
					break;
				case Parameter_Type::Trigger:
					if (ImGui::RadioButton("##trigger", parameter.second.value_bool))
					{
						parameter.second.value_bool = !parameter.second.value_bool;
						if (Auto_Save) Save_As();
					}
					break;
			}

			ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 20.0f);
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
			if (Auto_Save) Save_As();
		}
		ImGui::EndChild();
	}

	ImGui::SameLine();

	{
		ImGui::BeginChild("States", ImVec2(0, 0), true);

		const float input_padding = ImGui::GetWindowContentRegionWidth() * 0.4f;
		const float input_size = ImGui::GetWindowContentRegionWidth() - input_padding;

		shared_ptr<Animator_State_Machine> current_state;
		bool has_state = !state_machines.empty();

		if (has_state)
		{
			current_state = state_machines[current_state_index];
			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.8f);
			if (ImGui::BeginCombo("##State_Select", current_state->name.data()))
			{
				const size_t state_size = state_machines.size();
				for (size_t i = 0; i < state_size; ++i)
				{
					const bool is_selected = (current_state_index == i);
					if (ImGui::Selectable(state_machines[i]->name.data(), is_selected))
					{
						current_state_index = i;
						current_transition_index = 0;
					}

					if (is_selected) ImGui::SetItemDefaultFocus();
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
		if (ImGui::Button(u8"追加##ステート"))
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
				if (Auto_Save) Save_As();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (has_state)
		{
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.0f, 0.0f, 1.0f });
			if (ImGui::Button(u8"削除##ステート"))
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
					if (Auto_Save) Save_As();
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
			ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			if (ImGui::CollapsingHeader(u8"ステート設定"))
			{
				ImGui::Indent();
				ImGui::Text(u8"クリップ");
				ImGui::SameLine(input_padding);
				string clip_name;
				if (current_state->clip)
				{
					clip_name = current_state->clip->name;
				}
				else
				{
					clip_name = u8"クリップが選択されていません";
				}
				if (ImGui::Button(clip_name.c_str(), ImVec2(200, 0)))
				{
					string path = System_Function::Get_Open_File_Name("anim", "\\Resouces\\Model");
					if (!path.empty())
					{
						current_state->Set_Clip(path);
						if (Auto_Save) Save_As();
					}
				}

				ImGui::Text(u8"再生速度");
				ImGui::SameLine(input_padding);
				ImGui::SetNextItemWidth(input_size);
				if (ImGui::InputFloat("##Speed", &current_state->animation_speed))
				{
					if (Auto_Save) Save_As();
				}

				ImGui::Indent();
				ImGui::Text(u8"乗数");
				ImGui::SameLine(input_padding);
				ImGui::SetNextItemWidth(input_size);
				if (ImGui::InputText("##Multiply", &current_state->multiplier_hash))
				{
					if (Auto_Save) Save_As();
				}
				ImGui::Unindent();

				ImGui::Text(u8"開始フレーム");
				ImGui::SameLine(input_padding);
				ImGui::SetNextItemWidth(input_size);
				if (ImGui::DragInt("##Start_Frame", &current_state->start_frame, 1, 0, 0, "%d", ImGuiSliderFlags_AlwaysClamp))
				{
					if (Auto_Save) Save_As();
				}

				ImGui::Text(u8"終了フレーム");
				ImGui::SameLine(input_padding);
				ImGui::SetNextItemWidth(input_size);
				if (ImGui::DragInt("##End_Frame", &current_state->end_frame, 1, 0, 0, "%d", ImGuiSliderFlags_AlwaysClamp))
				{
					if (Auto_Save) Save_As();
				}

				ImGui::Text(u8"ループアニメーション");
				ImGui::SameLine(input_padding);
				if (ImGui::Checkbox("##Loop", &current_state->loopAnimation))
				{
					if (Auto_Save) Save_As();
				}

				ImGui::Text(u8"デフォルトステート");
				ImGui::SameLine(input_padding);
				if (ImGui::Checkbox("##Default", &current_state->is_default_state))
				{
					if (Auto_Save) Save_As();
				}

				ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
				if (ImGui::CollapsingHeader(u8"アニメーションイベント設定"))
				{
					ImGui::Indent();
					const bool has_event = !current_state->animation_events.empty();
					if (has_event)
					{
						bool erase_event = false;
						size_t erase_event_index = 0;
						const float window_width = ImGui::GetWindowContentRegionWidth();

						for (size_t i = 0; i < current_state->animation_events.size(); ++i)
						{
							Animation_Event& eve = current_state->animation_events[i];
							ImGui::PushID(i);

							ImGui::Text(u8"フレーム");
							ImGui::SameLine();
							ImGui::SetNextItemWidth(window_width * 0.1f);
							if (ImGui::InputInt("##animation_event_frame", &eve.frame))
							{
								if (Auto_Save) Save_As();
							}

							ImGui::SameLine();
							ImGui::Text(u8" パラメータ");
							ImGui::SameLine();
							ImGui::SetNextItemWidth(window_width * 0.25f);
							if (ImGui::BeginCombo("##animation_event_key", eve.key.data()))
							{
								for (auto& parameter : (*parameters))
								{
									const bool is_selected = (eve.key == parameter.first);
									if (ImGui::Selectable(parameter.first.data(), is_selected))
									{
										eve.key = parameter.first;
										eve.parameter.type = parameter.second.type;
										if (Auto_Save) Save_As();
									}
									if (is_selected) ImGui::SetItemDefaultFocus();
								}
								ImGui::EndCombo();
							}

							ImGui::SameLine();
							ImGui::Text(u8" セット値");
							ImGui::SameLine();
							ImGui::SetNextItemWidth(window_width * 0.1f);
							switch (eve.parameter.type)
							{
								case Parameter_Type::Int:
									if (ImGui::InputInt("##animation_event_value", &eve.parameter.value_int))
									{
										if (Auto_Save) Save_As();
									}
									break;
								case Parameter_Type::Float:
									if (ImGui::InputFloat("##animation_event_value", &eve.parameter.value_float))
									{
										if (Auto_Save) Save_As();
									}
									break;
								case Parameter_Type::Bool:
									if (ImGui::Checkbox("##animation_event_value", &eve.parameter.value_bool))
									{
										if (Auto_Save) Save_As();
									}
									break;
								case Parameter_Type::Trigger:
									if (ImGui::RadioButton("##animation_event_value", eve.parameter.value_bool))
									{
										eve.parameter.value_bool = !eve.parameter.value_bool;
										if (Auto_Save) Save_As();
									}
									break;
							}

							ImGui::SameLine(window_width - 20.0f);
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.0f, 0.0f, 1.0f });
							if (ImGui::Button(u8" × "))
							{
								erase_event = true;
								erase_event_index = i;
							}
							ImGui::PopStyleColor(1);
							ImGui::PopID();
						}

						if (erase_event)
						{
							current_state->animation_events.erase(current_state->animation_events.begin() + erase_event_index);
							if (Auto_Save) Save_As();
						}
					}
					else
					{
						ImGui::Text(u8"アニメーションイベントが設定されていません");
					}

					ImGui::Dummy({ 0, 0 });
					ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 200.0f);
					if (ImGui::Button(u8"アニメーションイベント追加", ImVec2(200.0f, 0)))
					{
						current_state->Add_Animation_Event();
						if (Auto_Save) Save_As();
					}
					ImGui::Unindent();
				}

				ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
				if (ImGui::CollapsingHeader(u8"ステートイベント設定"))
				{
					ImGui::Indent();
					const bool has_event = !current_state->state_events.empty();
					if (has_event)
					{
						const char* event_type[] = { "Int", "Float", "Bool", "Trigger" };
						const char* state_type[] = { "Enter", "Exit" };
						bool erase_event = false;
						size_t erase_event_index = 0;
						const float window_width = ImGui::GetWindowContentRegionWidth();

						for (size_t i = 0; i < current_state->state_events.size(); ++i)
						{
							State_Event& eve = current_state->state_events[i];
							ImGui::PushID(i);

							ImGui::Text(u8"実行条件");
							ImGui::SameLine();
							ImGui::SetNextItemWidth(window_width * 0.1f);
							int current_state_type = static_cast<int>(eve.type);
							if (ImGui::Combo("##state_condition_type", &current_state_type, state_type, IM_ARRAYSIZE(state_type)))
							{
								eve.type = static_cast<State_Event_Type>(current_state_type);
								if (Auto_Save) Save_As();
							}

							ImGui::SameLine();
							ImGui::Text(u8" パラメータ");
							ImGui::SameLine();
							ImGui::SetNextItemWidth(window_width * 0.25f);
							if (ImGui::BeginCombo("##state_event_key", eve.key.data()))
							{
								for (auto& parameter : (*parameters))
								{
									const bool is_selected = (eve.key == parameter.first);
									if (ImGui::Selectable(parameter.first.data(), is_selected))
									{
										eve.key = parameter.first;
										eve.parameter.type = parameter.second.type;
										if (Auto_Save) Save_As();
									}
									if (is_selected) ImGui::SetItemDefaultFocus();
								}
								ImGui::EndCombo();
							}

							ImGui::SameLine();
							ImGui::Text(u8" セット値");
							ImGui::SameLine();
							ImGui::SetNextItemWidth(window_width * 0.1f);

							switch (eve.parameter.type)
							{
								case Parameter_Type::Int:
									if (ImGui::InputInt("##state_event_value", &eve.parameter.value_int))
									{
										if (Auto_Save) Save_As();
									}
									break;
								case Parameter_Type::Float:
									if (ImGui::InputFloat("##state_event_value", &eve.parameter.value_float))
									{
										if (Auto_Save) Save_As();
									}
									break;
								case Parameter_Type::Bool:
									if (ImGui::Checkbox("##state_event_value", &eve.parameter.value_bool))
									{
										if (Auto_Save) Save_As();
									}
									break;
								case Parameter_Type::Trigger:
									if (ImGui::RadioButton("##state_event_value", eve.parameter.value_bool))
									{
										eve.parameter.value_bool = !eve.parameter.value_bool;
										if (Auto_Save) Save_As();
									}
									break;
							}

							ImGui::SameLine(window_width - 20.0f);
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.0f, 0.0f, 1.0f });
							if (ImGui::Button(u8" × "))
							{
								erase_event = true;
								erase_event_index = i;
							}
							ImGui::PopStyleColor(1);
							ImGui::PopID();
						}

						if (erase_event)
						{
							current_state->state_events.erase(current_state->state_events.begin() + erase_event_index);
							if (Auto_Save) Save_As();
						}
					}
					else
					{
						ImGui::Text(u8"ステートイベントが設定されていません");
					}

					ImGui::Dummy({ 0, 0 });
					ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 200.0f);
					if (ImGui::Button(u8"ステートイベント追加", ImVec2(200.0f, 0)))
					{
						current_state->Add_State_Event();
						if (Auto_Save) Save_As();
					}
					ImGui::Unindent();
				}

				ImGui::Dummy({ 0, 20.0f });
				ImGui::Unindent();
			}

			ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			if (ImGui::CollapsingHeader(u8"遷移設定"))
			{
				ImGui::Indent();
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
							if (ImGui::Selectable(current_state->transitions[n]->next_state.lock()->name.data(), is_selected))
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
					ImGui::Text(u8"遷移設定がありません");
					current_transition_index = 0;
				}

				if (state_machines.size() >= 2)
				{
					ImGui::SameLine();
					if (ImGui::Button(u8"追加##遷移"))
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
							size_t state_size = state_machines.size();
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
						if (ImGui::Button(u8"登録") && current_state_index != next_state_index)
						{
							current_state->Add_Transition(state_machines[next_state_index]);
							current_transition_index = static_cast<int>(current_state->transitions.size() - 1);
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
						if (ImGui::Button(u8"削除##遷移"))
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
								if (Auto_Save) Save_As();
								ImGui::CloseCurrentPopup();
							}
							ImGui::SameLine();
							if (ImGui::Button(u8"キャンセル", ImVec2(150, 0))) { ImGui::CloseCurrentPopup(); }
							ImGui::EndPopup();
						}

						if (state_machines.size() >= 2)
						{
							ImGui::SameLine();
							if (ImGui::Button(u8"優先順位変更##遷移"))
							{
								ImGui::OpenPopup(u8"優先順位変更メニュー");
							}
							if (ImGui::BeginPopup(u8"優先順位変更メニュー"))
							{
								static int swap_state_index = 0;

								if (ImGui::BeginCombo("##Swap_Select", current_state->transitions[swap_state_index]->next_state.lock()->name.data()))
								{
									const size_t state_size = current_state->transitions.size();
									for (size_t i = 0; i < state_size; ++i)
									{
										if (i != current_transition_index)
										{
											ImGui::PushID(i);
											const bool is_selected = (swap_state_index == i);
											if (ImGui::Selectable(current_state->transitions[i]->next_state.lock()->name.data(), is_selected))
											{
												swap_state_index = i;
											}
											if (is_selected) ImGui::SetItemDefaultFocus();

											ImGui::PopID();
										}
									}
									ImGui::EndCombo();
								}
								if (ImGui::Button(u8"交換") && current_transition_index != swap_state_index)
								{
									iter_swap(current_state->transitions.begin() + current_transition_index, current_state->transitions.begin() + swap_state_index);
									current_transition_index = swap_state_index;
									swap_state_index = 0;
									if (Auto_Save) Save_As();
									ImGui::CloseCurrentPopup();
								}
								ImGui::EndPopup();
							}
						}
					}

					if (has_transition)
					{
						ImGui::Text(u8"判定時間あり");
						ImGui::SameLine(input_padding);
						if (ImGui::Checkbox("##exit", &current_transition->has_exit_time))
						{
							if (Auto_Save) Save_As();
						}

						if (current_transition->has_exit_time)
						{
							ImGui::Indent();
							ImGui::Text(u8"判定時間");
							ImGui::SameLine(input_padding);
							ImGui::SetNextItemWidth(input_size);
							if (ImGui::InputFloat("##exit_time", &current_transition->exit_time))
							{
								if (Auto_Save) Save_As();
							}
							ImGui::Unindent();
						}

						ImGui::Text(u8"遷移間隔(秒)");
						ImGui::SameLine(input_padding);
						ImGui::SetNextItemWidth(input_size);
						if (ImGui::InputFloat("##Duration", &current_transition->transition_duration))
						{
							if (Auto_Save) Save_As();
						}

						ImGui::Text(u8"遷移オフセット");
						ImGui::SameLine(input_padding);
						ImGui::SetNextItemWidth(input_size);
						if (ImGui::InputFloat("##offset", &current_transition->transition_offset))
						{
							if (Auto_Save) Save_As();
						}

						ImGui::Text(u8"中断要因");
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
						if (ImGui::CollapsingHeader(u8"遷移条件"))
						{
							const float window_width = ImGui::GetWindowContentRegionWidth();
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
												if (condition->type == Parameter_Type::Int || condition->type == Parameter_Type::Float)
												{
													condition->mode = Condition_Mode::Greater;
												}
												else if (condition->type == Parameter_Type::Bool || condition->type == Parameter_Type::Trigger)
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
										if (it->second.type != Parameter_Type::Trigger)
										{
											const char* mode_char[6] = { "True","False","Greater","Less","Equals","NotEquals" };
											ImGui::SameLine();
											ImGui::SetNextItemWidth(window_width * 0.2f);
											const int current_mode = static_cast<int>(condition->mode);
											if (ImGui::BeginCombo("##mode", mode_char[current_mode]))
											{
												int mode_start, mode_end = 0;
												if (it->second.type == Parameter_Type::Int) { mode_start = 2; mode_end = 6; }
												else if (it->second.type == Parameter_Type::Float) { mode_start = 2; mode_end = 4; }
												else if (it->second.type == Parameter_Type::Bool) { mode_start = 0; mode_end = 2; }

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

											if (it->second.type == Parameter_Type::Int)
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
											else if (it->second.type == Parameter_Type::Float)
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
									if (ImGui::Button(u8" × "))
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
								ImGui::Text(u8"条件がありません");
							}

							ImGui::Dummy({});
							ImGui::SameLine(window_width - 100.0f);
							if (ImGui::Button(u8"条件追加", ImVec2(100.0f, 0)))
							{
								if (!parameters->empty())
								{
									auto it = parameters->begin();
									Condition_Mode mode = Condition_Mode::Greater;
									if (it->second.type == Parameter_Type::Bool || it->second.type == Parameter_Type::Trigger)
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