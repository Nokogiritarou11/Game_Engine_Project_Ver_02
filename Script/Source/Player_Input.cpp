#include "Player_Input.h"

using namespace std;
using namespace BeastEngine;

void Player_Input::Awake()
{
	animator = Get_Component<Animator>();
}

void Player_Input::Set_State()
{
	auto& anim = animator.lock();

	//��U��
	if (Input::Get_Pad_Button_Down(Button_Code::X))
	{
		anim->Set_Int("Attack_State", 1);
	}
	//���U��
	if (Input::Get_Pad_Button_Down(Button_Code::Y))
	{
		anim->Set_Int("Attack_State", 2);
	}
	//�W�����v
	if (Input::Get_Pad_Button_Down(Button_Code::A))
	{
		anim->Set_Trigger("Jump");
	}

}

bool Player_Input::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Player_Input", open)) return false;

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"�ݒ�ł���p�����[�^�͂���܂���");
	}
	return true;
}