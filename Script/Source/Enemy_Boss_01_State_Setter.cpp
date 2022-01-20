#include "Enemy_Boss_01_State_Setter.h"
#include "Enemy_Parameter.h"
#include "Editor.h"
#include "Enemy_Manager.h"
#include "Engine.h"
#include "Object_Pool.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Boss_01_State_Setter::Awake()
{
	//�����o�|�C���^�̎擾
	animator = Get_Component<Animator>();
	parameter = Get_Component<Enemy_Parameter>();
	target_transform = GameObject::Find_With_Tag("player").lock()->transform;
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
	pool = enemy_manager.lock()->Get_Component<Object_Pool>();
	Change_Attack_State();
}

void Enemy_Boss_01_State_Setter::Set_State()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();
	const auto& target = target_transform.lock();

	if (action_state == Action_State::Preparation)
	{
		//�v���C���[�Ƃ̋������U�������ɒB����܂Œ�������
		const Vector3 self_pos = transform->Get_Position();
		Vector3 target_pos = target->Get_Position();
		target_pos.y = self_pos.y;

		bool end_move = false;
		switch (preparation_state)
		{
			case Preparation_State::Chase:
				end_move = Vector3::DistanceSquared(target_pos, self_pos) < powf(preparation_distance, 2);
				break;
			case Preparation_State::Leave:
				end_move = Vector3::DistanceSquared(target_pos, self_pos) > powf(preparation_distance, 2);
				const Vector3 start_pos = transform->Get_Position() + Vector3(0, 0.5f, 0);
				if (Vector3 hit_pos; !end_move && Physics::Raycast(start_pos, start_pos - transform->Get_Forward() * 1.5f, hit_pos, 1 << 0)) end_move = true;
				break;
		}

		if (end_move)
		{
			//�����ɒB�����̂ōU�����s��
			action_state = Action_State::Attack;
			anim->Set_Trigger("Attack");
			anim->Set_Bool("Move", false);
		}
	}
	else if (action_state == Action_State::Attack)
	{
		if (anim->Get_Bool("Attack_End"))
		{
			anim->Set_Bool("Attack_End", false);
			Change_Attack_State();
			action_state = Action_State::Preparation;
		}
	}

	//Animator����U��������Ԃ��؂�ւ�������𔻒f����
	if (const bool attack_preliminary = anim->Get_Bool("Attack_Preliminary"); param->is_attack_preliminary != attack_preliminary)
	{
		param->is_attack_preliminary = attack_preliminary;
		if (param->is_attack_preliminary)
		{
			//�U����ԃ��X�g�Ɏ��g��ǉ�����
			enemy_manager.lock()->Add_Attacking_List(parameter);
			flash_particle.lock()->Set_Active(true);
		}
		else
		{
			//�U����Ԃ���������
			enemy_manager.lock()->Remove_Attacking_List(parameter);
		}
	}

	if (const bool stun = anim->Get_Bool("Stunning"); param->stunning != stun)
	{
		param->stunning = stun;
		if (stun)
		{
			enemy_manager.lock()->Add_Stunning_List(parameter);
		}
		else
		{
			//�X�^���I������
			enemy_manager.lock()->Remove_Stunning_List(parameter);
		}
	}

	if (anim->Get_Bool("Explosion"))
	{
		//���S���̔�������
		anim->Set_Bool("Explosion", false);
		//���S���Ă��邩�m�F
		if (param->hp <= 0)
		{
			//�����G�t�F�N�g���Đ�
			pool.lock()->Instance_In_Pool("Explosion_01", spine_transform.lock()->Get_Position(), transform->Get_Rotation());
			param->living = false;
			//�I�u�W�F�N�g�v�[���ɖ߂�(��A�N�e�B�u��)
			gameobject->Set_Active(false);
		}
	}

	//�X�[�p�[�A�[�}�[��Ԃ�Animator����X�V
	param->is_super_armor = anim->Get_Bool("Super_Armor");
}

void Enemy_Boss_01_State_Setter::Change_Attack_State()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	//�s����I������
	const int state = Mathf::Random_Range(0, 3);
	param->attack_state = state;
	attack_state = static_cast<Attack_State>(state);
	const auto& action = attack_action[state];

	preparation_state = action.preparation_state;
	preparation_distance = Mathf::Random_Range(action.preparation_distance_min, action.preparation_distance_max);

	anim->Set_Bool("Move", true);
	anim->Set_Int("Move_State", static_cast<int>(preparation_state));
	anim->Set_Int("Attack_State", state);

	if (preparation_state == Preparation_State::Chase) param->move_state = Move_State::Forward;
	if (preparation_state == Preparation_State::Leave) param->move_state = Move_State::Back;
}

bool Enemy_Boss_01_State_Setter::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Boss_01_State_Setter", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		{
			ImGui::PushID("center_bone");
			ImGui::Text(u8"���S�{�[��");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);

			string label_bone = u8"���ݒ� (�����Ƀh���b�O)";
			if (const auto& p = spine_transform.lock())
			{
				label_bone = p->gameobject->name;
			}
			ImGui::InputText("##Item", &label_bone, ImGuiInputTextFlags_ReadOnly);

			if (ImGui::BeginDragDropTarget())
			{
				if (const auto& drag = Engine::editor->Get_Drag_Object())
				{
					spine_transform = drag->transform;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
		}

		{
			ImGui::PushID("flash_pos");
			ImGui::Text(u8"�U���G�t�F�N�g���W");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);

			string label_flush = u8"���ݒ� (�����Ƀh���b�O)";
			if (const auto& p = flash_particle.lock())
			{
				label_flush = p->name;
			}
			ImGui::InputText("##Item", &label_flush, ImGuiInputTextFlags_ReadOnly);

			if (ImGui::BeginDragDropTarget())
			{
				if (const auto& drag = Engine::editor->Get_Drag_Object())
				{
					flash_particle = drag;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
		}

		if (ImGui::TreeNode(u8"�U�����s���ݒ�"))
		{
			const char* attack_types[] = { "Melee_3_Combo", "Melee_Once", "Slash_Dash","Slash_Jump" };
			const char* preparation_types[] = { "Chase","Leave" };
			for (int i = 0; i < 4; ++i)
			{
				if (ImGui::TreeNode(attack_types[i]))
				{
					auto& action = attack_action[i];
					int select = static_cast<int>(action.preparation_state);
					if (ImGui::LeftText_Combo(u8"�_���[�W�^�C�v", "##Preparation_State", &select, preparation_types, 2, window_center))
					{
						action.preparation_state = static_cast<Preparation_State>(select);
					}
					ImGui::LeftText_DragFloat(u8"�ŏ���������", "##preparation_distance_min", &action.preparation_distance_min, window_center);
					ImGui::LeftText_DragFloat(u8"�ő叀������", "##preparation_distance_max", &action.preparation_distance_max, window_center);

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}
	return true;
}