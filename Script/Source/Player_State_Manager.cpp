#include "Player_State_Manager.h"
#include "Character_Parameter.h"
#include "Character_Mover.h"

using namespace std;
using namespace BeastEngine;

void Player_State_Manager::Awake()
{
	parameter = Get_Component<Character_Parameter>();
	player_move = Get_Component<Character_Mover>();
	animator = Get_Component<Animator>();
}

void Player_State_Manager::Update()
{
	auto& anim = animator.lock();
	auto& param = parameter.lock();
	auto& p_move = player_move.lock();

	player_state = anim->Get_Int("Player_State");

	param->moveing = false;
	param->attacking = false;
	param->dodging = false;
	param->damaging = false;
	param->guarding = false;

	switch (player_state)
	{
		case 0:
			break;

		case 1:
			param->moveing = true;
			break;

		case 2:
			param->attacking = true;
			break;

		default:
			break;
	}

	//弱攻撃
	if (Input::Get_Pad_Button_Down(Button_Code::X))
	{
		anim->Set_Trigger("Attack");
	}

	//ステート別
	if (!param->eventing)
	{
		if (param->living)
		{
			if (param->attacking)
			{
				p_move->Move_Attack();
			}
			else if (param->dodging)
			{
				p_move->Move_Dodge();
			}
			else if (param->damaging)
			{
				p_move->Move_Damage();
			}
			else if (param->guarding)
			{
				p_move->Move_Guard();
			}
			else
			{
				p_move->Move_Normal();
			}
		}
	}
}

bool Player_State_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Player_State_Manager", open)) return false;

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"設定できるパラメータはありません");
	}
	return true;
}