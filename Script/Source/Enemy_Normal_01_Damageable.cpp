#include "Enemy_Normal_01_Damageable.h"
#include "Character_Hit_Stop_Manager.h"
#include "Editor.h"
#include "Engine.h"
#include "Enemy_Parameter.h"
#include "Enemy_Manager.h"
#include "Damage_Collision.h"
#include "Object_Pool.h"
#include "Player_Camera_Controller.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Normal_01_Damageable::Awake()
{
	//�����o�|�C���^�̎擾
	animator = Get_Component<Animator>();
	parameter = Get_Component<Enemy_Parameter>();
	hit_stop_manager = Get_Component<Character_Hit_Stop_Manager>();

	const auto& manager = GameObject::Find_With_Tag("Game_Manager").lock();
	pool = manager->Get_Component<Object_Pool>();
	enemy_manager = manager->Get_Component<Enemy_Manager>();
	camera_controller = GameObject::Find_With_Tag("main_camera").lock()->transform->Get_Parent().lock()->Get_Component<Player_Camera_Controller>();
}

bool Enemy_Normal_01_Damageable::Take_Damage(const shared_ptr<Damage_Collision>& damage_collision)
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
	hit_stop_manager.lock()->Start_Hit_Stop(damage_collision->hit_stop_time);
	//�v���C���[�̍ŏI�U���Ώۂ����g�ɐݒ肷��
	enemy_manager.lock()->last_attack_target = parameter;
	//�q�b�g���J�����V�F�C�N
	camera_controller.lock()->Shake_Camera(damage_collision->shake_count, damage_collision->shake_power);
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
			//�Ȍ�̃_���[�W����𖳌���
			param->is_invincible = true;
			//Animator�ɒʒm
			anim->Set_Trigger("Dead");
			anim->Set_Trigger("Damage");
			anim->Set_Int("Damage_State", static_cast<int>(damage_collision->damage_type));
			//�v���G�t�F�N�g���Đ�����
			pool.lock()->Instance_In_Pool(critical_particle_key, hit_pos_transform.lock()->Get_Position(), damage_collision->hit_transform.lock()->Get_Rotation());
			//���S������
			enemy_manager.lock()->Enemy_Dead(true, parameter);
			return true;
		}

		if (param->stun <= 0)
		{
			anim->Set_Trigger("Stun");
			enemy_manager.lock()->Enemy_Stunned();
			return true;
		}
	}

	//Animator�ɒʒm
	anim->Set_Trigger("Damage");
	anim->Set_Int("Damage_State", static_cast<int>(damage_collision->damage_type));
	return true;
}

bool Enemy_Normal_01_Damageable::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Normal_01_Damageable", open)) return false;

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