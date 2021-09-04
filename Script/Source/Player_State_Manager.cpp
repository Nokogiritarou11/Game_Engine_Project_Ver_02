#include "Player_State_Manager.h"
#include "Character_Parameter.h"
#include "Player_Move.h"

using namespace std;
using namespace BeastEngine;

void Player_State_Manager::Awake()
{
	parameter = Get_Component<Character_Parameter>();
	player_move = Get_Component<Player_Move>();
	animator = Get_Component<Animator>();
}

void Player_State_Manager::Update()
{
	Get_Input();

	auto& param = parameter.lock();
	auto& p_move = player_move.lock();

	if (!param->Eventing)
	{
		if (param->Living)
		{
			if (param->Attacking)
			{
				p_move->Move_Attack();
			}
			else if (param->Dodging)
			{
				p_move->Move_Dodge();
			}
			else if (param->Damaging)
			{
				p_move->Move_Damage();
			}
			else
			{
				p_move->Move_Normal();
			}
		}
	}
}

void Player_State_Manager::Get_Input()
{
	auto& anim = animator.lock();

	if (Input::Get_Pad_Button_Down(Button_Code::X))
	{
		anim->Set_Trigger("Attack");
	}
}

bool Player_State_Manager::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Player_State_Manager", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	if (ImGui::BeginPopupContextItem("Player_State_Manager_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Player_State_Manager>(shared_from_this()));
			removed = true;
		}
		ImGui::EndPopup();
	}
	if (removed)
	{
		return false;
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 20.0f);
	bool enable = Get_Enabled();
	if (ImGui::Checkbox("##enable", &enable))
	{
		Set_Enabled(enable);
	}

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.6f;
		ImGui::Text(u8"設定できるパラメータはありません");
	}
	return true;
}