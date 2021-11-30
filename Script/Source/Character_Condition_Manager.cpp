#include "Character_Condition_Manager.h"
#include "Character_Parameter.h"
#include "Interface_Character_Mover.h"
#include "Interface_Character_State_Setter.h"

using namespace std;
using namespace BeastEngine;

void Character_Condition_Manager::Awake()
{
	parameter = Get_Component<Character_Parameter>();
	character_move = Get_Component<Interface_Character_Mover>();
	character_state_setter = Get_Component<Interface_Character_State_Setter>();
	animator = Get_Component<Animator>();
}

void Character_Condition_Manager::Update()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();
	const auto& c_move = character_move.lock();
	const auto& c_state = character_state_setter.lock();

	c_state->Set_State();
	c_move->Move_Update();

	if (const int character_state = anim->Get_Int("Character_State"); character_state_old != character_state)
	{
		character_state_old = character_state;

		param->moving = false;
		param->attacking = false;
		param->damaging = false;
		param->guarding = false;

		switch (character_state)
		{
			case 0:
				break;

			case 1:
				param->moving = true;
				break;

			case 2:
				param->attacking = true;
				break;

			case 3:
				param->damaging = true;
				break;

			case 4:
				param->guarding = true;
				param->just_guard_timer = param->max_just_guard_time;
				break;

			default:
				break;
		}
	}

	//ステート別
	if (!param->pausing && param->living)
	{
		if (param->attacking)
		{
			c_move->Move_Attack();
		}
		else if (param->damaging)
		{
			c_move->Move_Damage();
		}
		else if (param->guarding)
		{
			c_move->Move_Guard();
		}
		else
		{
			c_move->Move_Normal();
		}
	}
}

bool Character_Condition_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Character_Condition_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"設定できるパラメータはありません");
	}
	return true;
}