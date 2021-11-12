#include "Enemy_Manager.h"
#include "Character_Parameter.h"
#include "Time_Manager.h"
#include "Player_Camera_Controller.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Manager::Awake()
{
	time_manager = Get_Component<Time_Manager>();
	camera_controller = GameObject::Find_With_Tag("main_camera").lock()->transform->Get_Parent().lock()->Get_Component<Player_Camera_Controller>();
}

void Enemy_Manager::Add_Enemy_List(const weak_ptr<Character_Parameter>& parameter)
{
	enemy_list.emplace_back(parameter);

	if(last_attack_target.expired())
	{
		last_attack_target = parameter;
	}
}

void Enemy_Manager::Add_Attacking_List(const std::weak_ptr<Character_Parameter>& parameter)
{
	attacking_list.emplace_back(parameter);
}

void Enemy_Manager::Remove_Attacking_List(const std::weak_ptr<Character_Parameter>& parameter)
{
	const auto& param = parameter.lock();
	const size_t size = attacking_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (param == attacking_list[i].lock())
		{
			swap(attacking_list[i], attacking_list.back());
			attacking_list.pop_back();
			break;
		}
	}
}

void Enemy_Manager::Remove_Stunning_List(const std::weak_ptr<Character_Parameter>& parameter)
{
	const auto& param = parameter.lock();
	const size_t size = stunning_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (param == stunning_list[i].lock())
		{
			swap(stunning_list[i], stunning_list.back());
			stunning_list.pop_back();
			break;
		}
	}
}

void Enemy_Manager::Enemy_Dead(const weak_ptr<Character_Parameter>& parameter)
{
	time_manager.lock()->Start_Time_Slow(dead_time_stop_delay, dead_time_stop_time, dead_time_stop_speed);
	camera_controller.lock()->Shake_Camera(dead_shake_camera_count, dead_shake_camera_power);

	const auto& param = parameter.lock();
	const size_t size = enemy_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (param == enemy_list[i].lock())
		{
			swap(enemy_list[i], enemy_list.back());
			enemy_list.pop_back();
			break;
		}
	}

	if (last_attack_target.lock() == param)
	{
		last_attack_target.reset();
	}
}

void Enemy_Manager::Enemy_Stunned(const weak_ptr<Character_Parameter>& parameter)
{
	stunning_list.emplace_back(parameter);
	time_manager.lock()->Start_Time_Slow(stun_time_stop_delay, stun_time_stop_time, stun_time_stop_speed);
	camera_controller.lock()->Shake_Camera(stun_shake_camera_count, stun_shake_camera_power);
}

Vector3 Enemy_Manager::Get_Nearest_Enemy_Position(const Vector3& position)
{
	Vector3 nearest_position;
	float nearest_distance = FLT_MAX;
	for (const auto& enemy : enemy_list)
	{
		const Vector3 pos = enemy.lock()->transform->Get_Position();
		if (const float dis = Vector3::DistanceSquared(pos, position); dis < nearest_distance)
		{
			nearest_distance = dis;
			nearest_position = pos;
		}
	}
	nearest_position.y = position.y;
	return nearest_position;
}

bool Enemy_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		if (ImGui::TreeNode(u8"敵死亡時演出パラメータ"))
		{
			if (ImGui::TreeNode(u8"スロー効果"))
			{
				ImGui::LeftText_DragFloat(u8"発生ディレイ", "##dead_time_stop_delay", &dead_time_stop_delay, window_center);
				ImGui::LeftText_DragFloat(u8"効果時間", "##dead_time_stop_time", &dead_time_stop_time, window_center);
				ImGui::LeftText_DragFloat(u8"速度", "##dead_time_stop_speed", &dead_time_stop_speed, window_center);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"カメラ揺れ"))
			{
				ImGui::LeftText_DragInt(u8"回数", "##dead_shake_camera_count", &dead_shake_camera_count, window_center);
				ImGui::LeftText_DragFloat(u8"強度", "##dead_shake_camera_power", &dead_shake_camera_power, window_center);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode(u8"敵スタン時演出パラメータ"))
		{
			if (ImGui::TreeNode(u8"スロー効果"))
			{
				ImGui::LeftText_DragFloat(u8"発生ディレイ", "##stun_time_stop_delay", &stun_time_stop_delay, window_center);
				ImGui::LeftText_DragFloat(u8"効果時間", "##stun_time_stop_time", &stun_time_stop_time, window_center);
				ImGui::LeftText_DragFloat(u8"速度", "##stun_time_stop_speed", &stun_time_stop_speed, window_center);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"カメラ揺れ"))
			{
				ImGui::LeftText_DragInt(u8"回数", "##stun_shake_camera_count", &stun_shake_camera_count, window_center);
				ImGui::LeftText_DragFloat(u8"強度", "##stun_shake_camera_power", &stun_shake_camera_power, window_center);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}
	return true;
}