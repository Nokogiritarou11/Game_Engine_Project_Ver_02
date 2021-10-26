#include "Enemy_Manager.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Manager::Add_Enemy_List(const weak_ptr<GameObject>& obj)
{
	enemy_list.emplace_back(obj);
}

void Enemy_Manager::Remove_Enemy_List(const weak_ptr<GameObject>& obj)
{
	const auto& obj_lock = obj.lock();
	const size_t size = enemy_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (obj_lock == enemy_list[i].lock())
		{
			swap(enemy_list[i], enemy_list.back());
			enemy_list.pop_back();
			break;
		}
	}
}

void Enemy_Manager::Add_Attacking_List(const weak_ptr<GameObject>& obj)
{
	attacking_list.emplace_back(obj);
}

void Enemy_Manager::Remove_Attacking_List(const weak_ptr<GameObject>& obj)
{
	const auto& obj_lock = obj.lock();
	const size_t size = attacking_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (obj_lock == attacking_list[i].lock())
		{
			swap(attacking_list[i], attacking_list.back());
			attacking_list.pop_back();
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
		ImGui::Text(u8"設定できるパラメータはありません");
	}
	return true;
}