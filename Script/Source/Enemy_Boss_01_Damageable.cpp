#include "Enemy_Boss_01_Damageable.h"
#include "Character_Hit_Stop_Manager.h"
#include "Editor.h"
#include "Engine.h"
#include "Enemy_Parameter.h"
#include "Enemy_Manager.h"
#include "Damage_Collision.h"
#include "Object_Pool.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Boss_01_Damageable::Awake()
{
	//�����o�|�C���^�̎擾
	animator = Get_Component<Animator>();
	parameter = Get_Component<Enemy_Parameter>();
	hit_stop_manager = Get_Component<Character_Hit_Stop_Manager>();

	const auto& manager = GameObject::Find_With_Tag("Game_Manager").lock();
	pool = manager->Get_Component<Object_Pool>();
	enemy_manager = manager->Get_Component<Enemy_Manager>();
}

bool Enemy_Boss_01_Damageable::Take_Damage(const shared_ptr<Damage_Collision>& damage_collision)
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	//���G��Ԃ̏ꍇ�̓��^�[��
	if (param->is_invincible) return false;

	//�X�[�p�[�A�[�}�[��ԂłȂ��ꍇ
	if (!param->is_super_armor)
	{
		//�v���C���[�̕��֌�������
		const Vector3 now_pos = transform->Get_Position();
		Vector3 look_pos = now_pos - damage_collision->root_transform.lock()->Get_Forward();
		look_pos.y = now_pos.y;
		transform->Set_Local_Rotation(transform->Look_At(look_pos));
	}

	//�q�b�g�X�g�b�v
	hit_stop_manager.lock()->Start_Hit_Stop(0.05f);
	//�v���C���[�̍ŏI�U���Ώۂ����g�ɐݒ肷��
	enemy_manager.lock()->last_attack_target = parameter;
	//�_���[�W���󂯂Ă���̃^�C�}�[�����Z�b�g����
	param->last_damaged_timer = 0;

	if (param->guarding)
	{
		//�K�[�h��Ԃ������ꍇ
		//Animator�̃p�����[�^��ύX�����^�[��
		anim->Set_Trigger("Damage");
		return true;
	}

	//�q�b�g�G�t�F�N�g���Đ�����
	pool.lock()->Instance_In_Pool(damage_collision->hit_particle_key, hit_pos_transform.lock()->Get_Position(), damage_collision->hit_transform.lock()->Get_Rotation());
	//������Ԏ��̂ݍs��
	if (param->hp > 0)
	{
		//HP�ւ̃_���[�W����
		param->hp -= damage_collision->damage_hp;

		//�X�^���_���[�W����
		if (param->is_ground && !param->stunning)
		{
			param->stun -= damage_collision->damage_stun;
		}

		//�_���[�W�ɂ��v����
		if (param->hp <= 0)
		{
			//�{�X�Ȃ̂ŃX�^���t�B�j�b�V���݂̂Ŏ��S����
			param->stunning = true;
			anim->Set_Trigger("Stun");
			enemy_manager.lock()->Enemy_Stunned(true, parameter);
			return true;
		}

		if (param->stun <= 0)
		{
			param->stunning = true;
			anim->Set_Trigger("Stun");
			enemy_manager.lock()->Enemy_Stunned(true, parameter);
			return true;
		}
	}

	return true;
}

bool Enemy_Boss_01_Damageable::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Boss_01_Damageable", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::Text(u8"�q�b�g�G�t�F�N�g�ʒu");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);

		string label_parent = u8"���ݒ� (�����Ƀh���b�O)";
		if (const auto& p = hit_pos_transform.lock())
		{
			label_parent = p->gameobject->name;
		}
		ImGui::InputText("##Item", &label_parent, ImGuiInputTextFlags_ReadOnly);

		if (ImGui::BeginDragDropTarget())
		{
			if (const auto& drag = Engine::editor->Get_Drag_Object())
			{
				hit_pos_transform = drag->transform;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::LeftText_InputText(u8"�v���G�t�F�N�g�L�[", "##critical_particle_key", &critical_particle_key, window_center);
	}
	return true;
}