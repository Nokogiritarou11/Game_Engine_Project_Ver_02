#include "Enemy_Damageable_Normal.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Damageable_Normal::Awake()
{
	animator = Get_Component<Animator>();
	parameter = Get_Component<Character_Parameter>();
}

void Enemy_Damageable_Normal::Take_Damage(const int& damage_hp, const int& damage_stun, const Vector3& from_pos, const Damage_Type& damage_state)
{

}

bool Enemy_Damageable_Normal::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Damageable_Normal", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"İ’è‚Å‚«‚éƒpƒ‰ƒ[ƒ^‚Í‚ ‚è‚Ü‚¹‚ñ");
	}
	return true;
}