#include "Tutorial_Stage.h"
#include "Sound_Manager.h"
#include "Scene_Change_Manager.h"
#include "Character_Condition_Manager.h"
#include "Enemy_Manager.h"
#include "Engine.h"
#include "Editor.h"

using namespace std;
using namespace BeastEngine;

void Tutorial_Stage::Awake()
{
	GameObject::Find("Sound_Manager").lock()->Get_Component<Sound_Manager>()->Play_BGM(BGM_Name::Normal_Battle);
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
	condition_manager = GameObject::Find_With_Tag("player").lock()->Get_Component<Character_Condition_Manager>();
}

void Tutorial_Stage::Update()
{
	if (tutorial_state != Tutorial_State::Scene_Change)
	{
		if (Input::Get_Pad_Button_Down(Button_Code::Start))
		{
			tutorial_state = Tutorial_State::Scene_Change;
		}
	}

	if (tutorial_state == Tutorial_State::Move && frame_count < 2)
	{
		++frame_count;
		return;
	}

	if (tutorial_state == Tutorial_State::Move || tutorial_state == Tutorial_State::Parry)
	{
		switch (view_state)
		{
			case View_State::Wait:
			{
				timer += Time::delta_time;
				if (wait_time <= timer)
				{
					stop_sprite[static_cast<int>(tutorial_state)].lock()->Set_Active(true);
					condition_manager.lock()->Set_Enabled(false);
					view_state = View_State::Stop;
					Time::time_scale = 0;
					timer = 0;
				}
			}
			break;
			case View_State::Stop:
			{
				if (Input::Get_Pad_Button_Down(Button_Code::A))
				{
					stop_sprite[static_cast<int>(tutorial_state)].lock()->Set_Active(false);
					playing_sprite[static_cast<int>(tutorial_state)].lock()->Set_Active(true);
					Time::time_scale = 1;
					enemy_manager.lock()->Instance_Enemy(Enemy_Type::Enemy_Normal_01, transform->Get_Position(), transform->Get_Rotation());
					view_state = View_State::Close;
				}
			}
			break;
			case View_State::Close:
			{
				condition_manager.lock()->Set_Enabled(true);
				view_state = View_State::Playing;
			}
			break;
			case View_State::Playing:
			{
				if (enemy_manager.lock()->enemy_list.empty())
				{
					timer += Time::delta_time;
					if (end_time <= timer)
					{
						timer = 0;
						view_state = View_State::Wait;
						playing_sprite[static_cast<int>(tutorial_state)].lock()->Set_Active(false);
						if (tutorial_state == Tutorial_State::Move) tutorial_state = Tutorial_State::Parry;
						else if (tutorial_state == Tutorial_State::Parry) tutorial_state = Tutorial_State::Scene_Change;
					}
				}
			}
			break;
		}
	}
	else if (tutorial_state == Tutorial_State::Scene_Change && !is_changing)
	{
		is_changing = true;
		GameObject::Find("Scene_Change_Manager").lock()->Get_Component<Scene_Change_Manager>()->Change_Scene("Assets\\Scene\\Main_Stage.bin");
	}
}

bool Tutorial_Stage::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Tutorial_Stage", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		if (ImGui::TreeNode(u8"チュートリアルデータ"))
		{
			const string tutorial_name[2] = { u8"基本操作", u8"パリィ&ロックオン" };
			for (int i = 0; i < 2; ++i)
			{
				ImGui::PushID(i);

				if (ImGui::TreeNode(tutorial_name[i].c_str()))
				{
					ImGui::Text(u8"一時停止説明");
					ImGui::SameLine(window_center);
					ImGui::SetNextItemWidth(-FLT_MIN);
					{
						string label_stop = u8"未設定 (ここにドラッグ)";
						if (const auto& p = stop_sprite[i].lock())
						{
							label_stop = p->name;
						}
						ImGui::InputText("##Item_stop", &label_stop, ImGuiInputTextFlags_ReadOnly);

						if (ImGui::BeginDragDropTarget())
						{
							if (const auto& drag = Engine::editor->Get_Drag_Object())
							{
								stop_sprite[i] = drag;
							}
							ImGui::EndDragDropTarget();
						}
					}

					ImGui::Text(u8"常時表示説明");
					ImGui::SameLine(window_center);
					ImGui::SetNextItemWidth(-FLT_MIN);
					{
						string label_playing = u8"未設定 (ここにドラッグ)";
						if (const auto& p = playing_sprite[i].lock())
						{
							label_playing = p->name;
						}
						ImGui::InputText("##Item_playing", &label_playing, ImGuiInputTextFlags_ReadOnly);

						if (ImGui::BeginDragDropTarget())
						{
							if (const auto& drag = Engine::editor->Get_Drag_Object())
							{
								playing_sprite[i] = drag;
							}
							ImGui::EndDragDropTarget();
						}
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		ImGui::LeftText_DragFloat(u8"開始待機時間", "##wait_time", &wait_time, window_center);
		ImGui::LeftText_DragFloat(u8"終了待機時間", "##end_time", &end_time, window_center);
	}
	return true;
}