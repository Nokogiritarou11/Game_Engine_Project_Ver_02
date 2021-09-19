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
	auto& anim = animator.lock();
	auto& param = parameter.lock();
	auto& c_move = character_move.lock();
	auto& c_state = character_state_setter.lock();

	param->moveing = false;
	param->attacking = false;
	param->dodging = false;
	param->damaging = false;
	param->guarding = false;

	switch (anim->Get_Int("Character_State"))
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

	c_state->Set_State();
	c_move->Ground_Update();

	//ステート別
	if (!param->eventing)
	{
		if (!param->pausing)
		{
			if (param->living)
			{
				if (param->attacking)
				{
					c_move->Move_Attack();
				}
				else if (param->dodging)
				{
					c_move->Move_Dodge();
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
	}
}

bool Character_Condition_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Character_Condition_Manager", open)) return false;

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"設定できるパラメータはありません");
	}
	return true;
}