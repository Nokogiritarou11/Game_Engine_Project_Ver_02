#include "Player_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Player_Parameter::OnEnable()
{
	//アクティブ時にメンバ変数をリセットする(オブジェクトプールから呼び出されたときなど)
	Character_Parameter_Reset();
	camera_locking = false;
}

void Player_Parameter::Update()
{
	//ジャストガードが成立してからの時間を計測する
	if (just_guard_timer > 0)
	{
		just_guard_timer -= Time::delta_time;
		if (just_guard_timer <= 0)
		{
			just_guard_timer = 0;
		}
	}
}

bool Player_Parameter::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Player_Parameter", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat("Max_HP", "##Max_HP", &max_hp, window_center, -FLT_MIN, 1.0f);
		ImGui::LeftText_DragFloat("Max_Stun", "##Max_Stun", &max_stun, window_center, -FLT_MIN, 1.0f);
		ImGui::LeftText_DragFloat(u8"スタン回復量", "##heal_stun", &heal_stun, window_center);

		ImGui::Text("HP");
		ImGui::SameLine(window_center);
		ImGui::Text("%01f", hp);
		ImGui::Text(u8"スタン耐久値");
		ImGui::SameLine(window_center);
		ImGui::Text("%01f", stun);

		ImGui::LeftText_Checkbox(u8"生存中", "##living", &living, window_center);
		ImGui::LeftText_Checkbox(u8"移動中", "##moving", &moving, window_center);
		ImGui::LeftText_Checkbox(u8"攻撃中", "##attacking", &attacking, window_center);
		ImGui::LeftText_Checkbox(u8"被ダメ中", "##damaging", &damaging, window_center);
		ImGui::LeftText_Checkbox(u8"接地中", "##is_ground", &is_ground, window_center);

		ImGui::LeftText_DragFloat(u8"弾き猶予時間", "##Just_Guard_Time", &max_just_guard_time, window_center);
	}
	return true;
}