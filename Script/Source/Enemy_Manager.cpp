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
	for(size_t i = 0; i < size; ++i)
	{
		if(obj_lock == enemy_list[i].lock())
		{
			swap(enemy_list[i], enemy_list.back());
			enemy_list.pop_back();
			break;
		}
	}
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