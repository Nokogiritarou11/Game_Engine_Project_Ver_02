#include "Enemy_Damageable_Normal.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Damageable_Normal::Awake()
{
	animator = Get_Component<Animator>();
	parameter = Get_Component<Character_Parameter>();
}

void Enemy_Damageable_Normal::Take_Damage(int damage_hp, int damage_stun, Vector3 from_pos, Damage_Type damage_state)
{

}

bool Enemy_Damageable_Normal::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Damageable_Normal", open)) return false;

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"İ’è‚Å‚«‚éƒpƒ‰ƒ[ƒ^‚Í‚ ‚è‚Ü‚¹‚ñ");
	}
	return true;
}