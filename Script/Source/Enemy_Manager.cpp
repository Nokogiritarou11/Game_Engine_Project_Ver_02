#include "Enemy_Manager.h"
#include "Player_Parameter.h"
#include "Time_Manager.h"
#include "Player_Camera_Controller.h"
#include "Object_Pool.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Manager::Awake()
{
	time_manager = Get_Component<Time_Manager>();
	camera_controller = GameObject::Find_With_Tag("main_camera").lock()->transform->Get_Parent().lock()->Get_Component<Player_Camera_Controller>();
	pool = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Object_Pool>();
	player_parameter = GameObject::Find_With_Tag("player").lock()->Get_Component<Player_Parameter>();
}

void Enemy_Manager::Instance_Enemy(const Enemy_Type& type, const Vector3& position, const Quaternion& rotation)
{
	string key;

	switch (type)
	{
		case Enemy_Type::Enemy_Normal_01:
			key = "Enemy_Normal_01";
			break;
		case Enemy_Type::Enemy_Big_01:
			key = "Enemy_Big_01";
			break;
		default:
			return;
	}

	const auto& parameter = pool.lock()->Instance_In_Pool(key, position, rotation)->Get_Component<Enemy_Parameter>();
	enemy_list.emplace_back(parameter);

	if (last_attack_target.expired())
	{
		last_attack_target = parameter;
	}
}

void Enemy_Manager::Add_Attacking_List(const std::weak_ptr<Enemy_Parameter>& parameter)
{
	attacking_list.emplace_back(parameter);
}

void Enemy_Manager::Remove_Attacking_List(const std::weak_ptr<Enemy_Parameter>& parameter)
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

void Enemy_Manager::Remove_Stunning_List(const std::weak_ptr<Enemy_Parameter>& parameter)
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

void Enemy_Manager::Enemy_Dead(const bool& use_effect, const weak_ptr<Enemy_Parameter>& parameter)
{
	if (use_effect)
	{
		time_manager.lock()->Start_Time_Slow(effect_dead.time_parameter.delay, effect_dead.time_parameter.time, effect_dead.time_parameter.speed);
		camera_controller.lock()->Shake_Camera(effect_dead.shake_parameter.count, effect_dead.shake_parameter.power);
	}

	const auto& param = parameter.lock();
	param->hp = 0;

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

	const auto& p_param = player_parameter.lock();

	if (last_attack_target.lock() == param)
	{
		if (enemy_list.empty())
		{
			last_attack_target.reset();

		}
		else
		{
			float nearest_distance = FLT_MAX;
			for (const auto& enemy : enemy_list)
			{
				const Vector3 pos = enemy.lock()->transform->Get_Position();
				if (const float dis = Vector3::DistanceSquared(pos, p_param->transform->Get_Position()); dis < nearest_distance)
				{
					nearest_distance = dis;
					last_attack_target = enemy;
				}
			}
		}
	}

	if (p_param->target.lock() == param)
	{
		p_param->target.reset();
	}
}

void Enemy_Manager::Enemy_Stunned(const bool& use_effect, const weak_ptr<Enemy_Parameter>& parameter)
{
	stunning_list.emplace_back(parameter);

	if (use_effect)
	{
		time_manager.lock()->Start_Time_Slow(effect_stun.time_parameter.delay, effect_stun.time_parameter.time, effect_stun.time_parameter.speed);
		camera_controller.lock()->Shake_Camera(effect_stun.shake_parameter.count, effect_stun.shake_parameter.power);
	}
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
				ImGui::LeftText_DragFloat(u8"発生ディレイ", "##effect_dead_time_parameter_delay", &effect_dead.time_parameter.delay, window_center);
				ImGui::LeftText_DragFloat(u8"効果時間", "##effect_dead_time_parameter_time", &effect_dead.time_parameter.time, window_center);
				ImGui::LeftText_DragFloat(u8"速度", "##effect_dead_time_parameter_speed", &effect_dead.time_parameter.speed, window_center);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"カメラ揺れ"))
			{
				ImGui::LeftText_DragInt(u8"回数", "##effect_dead_shake_parameter_count", &effect_dead.shake_parameter.count, window_center);
				ImGui::LeftText_DragFloat(u8"強度", "##effect_dead_shake_parameter_power", &effect_dead.shake_parameter.power, window_center);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode(u8"敵スタン時演出パラメータ"))
		{
			if (ImGui::TreeNode(u8"スロー効果"))
			{
				ImGui::LeftText_DragFloat(u8"発生ディレイ", "##effect_stun_time_parameter_delay", &effect_stun.time_parameter.delay, window_center);
				ImGui::LeftText_DragFloat(u8"効果時間", "##effect_stun_time_parameter_time", &effect_stun.time_parameter.time, window_center);
				ImGui::LeftText_DragFloat(u8"速度", "##effect_stun_time_parameter_speed", &effect_stun.time_parameter.speed, window_center);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"カメラ揺れ"))
			{
				ImGui::LeftText_DragInt(u8"回数", "##effect_stun_shake_parameter_count", &effect_stun.shake_parameter.count, window_center);
				ImGui::LeftText_DragFloat(u8"強度", "##effect_stun_shake_parameter_power", &effect_stun.shake_parameter.power, window_center);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::Button(u8"敵_通常_01 生成"))
		{
			Instance_Enemy(Enemy_Type::Enemy_Normal_01, Vector3::Zero, Quaternion::Identity);
		}
	}
	return true;
}