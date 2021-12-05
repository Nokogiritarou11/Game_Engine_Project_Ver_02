#include "Enemy_Parameter.h"

#include "Editor.h"
#include "Engine.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Parameter::OnEnable()
{
	//アクティブ時にメンバ変数をリセットする(オブジェクトプールから呼び出されたときなど)
	Character_Parameter_Reset();
	stunning = false;
	is_attack_mode = false;
	is_attack_preliminary = false;
	last_damaged_timer = 0;
	move_state = Move_State::Forward;
}

void Enemy_Parameter::Update()
{
	//最後に攻撃されてからの経過時間を更新
	last_damaged_timer += Time::delta_time;
	//スタン値の回復
	if (stun < max_stun)
	{
		stun = Mathf::Clamp(stun + heal_stun * Time::delta_time, 0, max_stun);
	}
}

bool Enemy_Parameter::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Parameter", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		static const char* type_name[] = { "Enemy_Normal_01", "Enemy_Big_01", "Boss_01" };
		int type_current = static_cast<int>(type);
		if (ImGui::LeftText_Combo(u8"エネミータイプ", "##Enemy_Type", &type_current, type_name, IM_ARRAYSIZE(type_name), window_center))
		{
			type = static_cast<Enemy_Type>(type_current);
		}

		ImGui::Text(u8"ロックオンマーカー座標");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);

		string label_flush = u8"未設定 (ここにドラッグ)";
		if (const auto& p = lock_on_target.lock())
		{
			label_flush = p->gameobject->name;
		}
		ImGui::InputText("##Item", &label_flush, ImGuiInputTextFlags_ReadOnly);

		if (ImGui::BeginDragDropTarget())
		{
			if (const auto& drag = Engine::editor->Get_Drag_Object())
			{
				lock_on_target = drag->transform;
			}
			ImGui::EndDragDropTarget();
		}

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
	}
	return true;
}