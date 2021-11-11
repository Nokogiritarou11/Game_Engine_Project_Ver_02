#include "Enemy_Manager.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Manager::Awake()
{
	player_parameter = GameObject::Find_With_Tag("player").lock()->Get_Component<Character_Parameter>();
}

void Enemy_Manager::Add_Enemy_List(const weak_ptr<Character_Parameter>& parameter)
{
	enemy_list.emplace_back(parameter);

	if(last_attack_target.expired())
	{
		last_attack_target = parameter;
	}
}

void Enemy_Manager::Remove_Enemy_List(const weak_ptr<Character_Parameter>& parameter)
{
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

	if(last_attack_target.lock() == param)
	{
		last_attack_target.reset();
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

void Enemy_Manager::Add_Stunning_List(const std::weak_ptr<Character_Parameter>& parameter)
{
	stunning_list.emplace_back(parameter);
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
		ImGui::Text(u8"Ý’è‚Å‚«‚éƒpƒ‰ƒ[ƒ^‚Í‚ ‚è‚Ü‚¹‚ñ");
	}
	return true;
}