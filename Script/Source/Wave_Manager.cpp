#include "Wave_Manager.h"
using namespace std;
using namespace BeastEngine;

void Wave_Manager::Awake()
{
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
}

void Wave_Manager::Update()
{
	if (wave_count >= 0)
	{
		const auto& manager = enemy_manager.lock();
		if (!wave_finished)
		{
			wave_timer += Time::delta_time;
			if (const auto& wave = wave_data[wave_count]; wave.instance_delay <= wave_timer)
			{
				for (const auto& enemy : wave.enemy_data)
				{
					manager->Instance_Enemy(enemy.type, enemy.position, Quaternion::Identity);
				}
				wave_finished = true;
				wave_timer = 0;
				++wave_count;
			}
		}
		else
		{
			if (manager->enemy_list.empty())
			{
				if (++wave_count < static_cast<int>(wave_data.size()))
				{
					wave_finished = false;
				}
				else
				{
					for (const auto& f : wave_end_callback) f();
					gameobject->Set_Active(false);
				}
			}
		}
	}
}

void Wave_Manager::Add_Callback_Start(const std::function<void()>& callback)
{
	wave_start_callback.emplace_back(callback);
}

void Wave_Manager::Add_Callback_End(const std::function<void()>& callback)
{
	wave_end_callback.emplace_back(callback);
}

void Wave_Manager::OnTrigger_Enter(Collision& collision)
{
	++wave_count;
	for (const auto& f : wave_start_callback) f();
	Get_Component<Collider>()->Set_Enabled(false);
}

bool Wave_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Wave_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		if (ImGui::TreeNode(u8"ウェーブデータ"))
		{
			int wave_size = static_cast<int>(wave_data.size());

			ImGui::Text(u8"ウェーブ数");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##wave_count", &wave_size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				wave_data.resize(wave_size);
			}

			for (int i = 0; i < wave_size; ++i)
			{
				ImGui::PushID(i);

				const string label_wave = u8"ウェーブ: " + to_string(i);
				if (ImGui::TreeNode(label_wave.c_str()))
				{
					auto& enemy_data = wave_data[i].enemy_data;
					int instance_size = static_cast<int>(enemy_data.size());

					ImGui::Text(u8"エネミー数");
					ImGui::SameLine(window_center);
					ImGui::SetNextItemWidth(-FLT_MIN);
					if (ImGui::InputInt("##enemy_count", &instance_size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						enemy_data.resize(instance_size);
					}

					for (int j = 0; j < instance_size; ++j)
					{
						ImGui::PushID(j);

						const string label_enemy = u8"エネミー: " + to_string(j);
						if (ImGui::TreeNode(label_enemy.c_str()))
						{
							ImGui::LeftText_DragFloat3(u8"位置", "##start_position", enemy_data[j].position, window_center);
							static const char* type_name[] = { "Enemy_Normal_01", "Enemy_Big_01", "Boss_01" };
							int type_current = static_cast<int>(enemy_data[j].type);
							if (ImGui::LeftText_Combo(u8"エネミータイプ", "##Enemy_Type", &type_current, type_name, IM_ARRAYSIZE(type_name), window_center))
							{
								enemy_data[j].type = static_cast<Enemy_Type>(type_current);
							}

							ImGui::TreePop();
						}

						ImGui::PopID();
					}
					ImGui::LeftText_DragFloat(u8"出現ディレイ", "##change_time", &wave_data[i].instance_delay, window_center);

					ImGui::TreePop();
				}

				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}
	return true;
}