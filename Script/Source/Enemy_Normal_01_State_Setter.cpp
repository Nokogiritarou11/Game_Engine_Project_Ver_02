#include "Enemy_Normal_01_State_Setter.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Normal_01_State_Setter::Awake()
{
	animator = Get_Component<Animator>();
}

void Enemy_Normal_01_State_Setter::Set_State()
{
}

bool Enemy_Normal_01_State_Setter::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Normal_01_State_Setter", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"�ݒ�ł���p�����[�^�͂���܂���");
	}
	return true;
}