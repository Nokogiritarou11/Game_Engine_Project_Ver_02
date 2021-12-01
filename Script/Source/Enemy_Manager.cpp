#include "Enemy_Manager.h"
#include "Player_Parameter.h"
#include "Time_Manager.h"
#include "Player_Camera_Controller.h"
#include "Object_Pool.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Manager::Awake()
{
	//�����o�|�C���^�̎擾
	time_manager = Get_Component<Time_Manager>();
	camera_controller = GameObject::Find_With_Tag("main_camera").lock()->transform->Get_Parent().lock()->Get_Component<Player_Camera_Controller>();
	pool = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Object_Pool>();
	player_parameter = GameObject::Find_With_Tag("player").lock()->Get_Component<Player_Parameter>();
}

void Enemy_Manager::Update()
{
	//�O��U���w�����o���Ă���̌o�ߎ��Ԃ��X�V
	last_attacked_timer += Time::delta_time;

	//�U���w�����s���ꍇ
	if (last_attacked_timer >= attack_check_time && !enemy_list.empty())
	{
		//�U�����󂯂Ă��Ȃ��G�l�~�[�̒��Ńv���C���[�ɑ΂��Ĉ�ԋ߂����̂���������
		weak_ptr<Enemy_Parameter> nearest_enemy;
		const Vector3 player_pos = player_parameter.lock()->transform->Get_Position();
		float min_dis = FLT_MAX;
		for (const auto& enemy_weak : enemy_list)
		{
			if (const auto& enemy = enemy_weak.lock(); enemy->last_damaged_timer >= damaged_check_time && !enemy->stunning && !enemy->is_attack_preliminary)
			{
				const Vector3 pos = enemy->transform->Get_Position();
				if (const float dis = Vector3::DistanceSquared(pos, player_pos); dis < min_dis)
				{
					min_dis = dis;
					nearest_enemy = enemy_weak;
				}
			}
		}

		//�Ώۂ̑��݃`�F�b�N
		if (const auto& enemy = nearest_enemy.lock())
		{
			//�U���w�����o��
			enemy->is_attack_mode = true;
		}
		//�^�C�}�[�̃��Z�b�g
		last_attacked_timer = 0;
	}
}

void Enemy_Manager::Instance_Enemy(const Enemy_Type& type, const Vector3& position, const Quaternion& rotation)
{
	string key;
	//�����ɂ���ďo��������G�l�~�[��ς���
	switch (type)
	{
		case Enemy_Type::Enemy_Normal_01:
			key = "Enemy_Normal_01";
			break;
		case Enemy_Type::Enemy_Big_01:
			key = "Enemy_Big_01";
			break;
		default:
			return;
	}

	//�I�u�W�F�N�g�v�[������G�l�~�[���Ăяo���A���X�g�ɉ�����
	const auto& parameter = pool.lock()->Instance_In_Pool(key, position, rotation)->Get_Component<Enemy_Parameter>();
	enemy_list.emplace_back(parameter);

	//�ǉ��O�ɃA�N�e�B�u�ȃG�l�~�[�����Ȃ������ꍇ�̓��b�N�I���Ώۂɐݒ肷��
	if (last_attack_target.expired())
	{
		last_attack_target = parameter;
	}
}

void Enemy_Manager::Add_Attacking_List(const std::weak_ptr<Enemy_Parameter>& parameter)
{
	//�U�������X�g�ɒǉ�����
	attacking_list.emplace_back(parameter);
}

void Enemy_Manager::Remove_Attacking_List(const std::weak_ptr<Enemy_Parameter>& parameter)
{
	//���X�g����폜�Ώۂ�T���č폜����
	const auto& param = parameter.lock();
	const size_t size = attacking_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (param == attacking_list[i].lock())
		{
			swap(attacking_list[i], attacking_list.back());
			attacking_list.pop_back();
			break;
		}
	}
}

void Enemy_Manager::Remove_Stunning_List(const std::weak_ptr<Enemy_Parameter>& parameter)
{
	//���X�g����폜�Ώۂ�T���č폜����
	const auto& param = parameter.lock();
	const size_t size = stunning_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (param == stunning_list[i].lock())
		{
			swap(stunning_list[i], stunning_list.back());
			stunning_list.pop_back();
			break;
		}
	}
}

void Enemy_Manager::Enemy_Dead(const bool& use_effect, const weak_ptr<Enemy_Parameter>& parameter)
{
	//���S�����o���s���ꍇ
	if (use_effect)
	{
		//���Ԓ�~�ƃJ�����V�F�C�N���s��
		time_manager.lock()->Start_Time_Slow(effect_dead.time_parameter.delay, effect_dead.time_parameter.time, effect_dead.time_parameter.speed);
		camera_controller.lock()->Shake_Camera(effect_dead.shake_parameter.count, effect_dead.shake_parameter.power);
	}

	const auto& param = parameter.lock();
	//�����Z�̏ꍇ������̂�HP�������I��0�ɂ���
	param->hp = 0;

	//�������X�g����폜����
	const size_t size = enemy_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (param == enemy_list[i].lock())
		{
			swap(enemy_list[i], enemy_list.back());
			enemy_list.pop_back();
			break;
		}
	}

	const auto& p_param = player_parameter.lock();

	//���݃v���C���[�̍U���Ώۂ������ꍇ
	if (last_attack_target.lock() == param)
	{
		if (enemy_list.empty())
		{
			//�c��̐����G�l�~�[�����݂��Ȃ���΍U���Ώۃ|�C���^�����Z�b�g����
			last_attack_target.reset();

		}
		else
		{
			//�܂��G�l�~�[���c���Ă���̂Ńv���C���[�Ɉ�ԋ߂����̂����ɑI������
			float nearest_distance = FLT_MAX;
			for (const auto& enemy : enemy_list)
			{
				const Vector3 pos = enemy.lock()->transform->Get_Position();
				if (const float dis = Vector3::DistanceSquared(pos, p_param->transform->Get_Position()); dis < nearest_distance)
				{
					nearest_distance = dis;
					last_attack_target = enemy;
				}
			}
		}
	}

	//�U�����������ꍇ�̓��X�g����폜����
	if (param->is_attack_preliminary) Remove_Attacking_List(parameter);
	//�X�^�����������ꍇ�̓��X�g����폜����
	if (param->stunning) Remove_Stunning_List(parameter);

	//���݃v���C���[�̃��b�N�I���Ώۂ������ꍇ
	if (p_param->target.lock() == param)
	{
		//���b�N�I������������
		p_param->target.reset();
	}
}

void Enemy_Manager::Enemy_Stunned(const bool& use_effect, const weak_ptr<Enemy_Parameter>& parameter)
{
	//�X�^����ԃ��X�g�ɒǉ�
	stunning_list.emplace_back(parameter);

	//�X�^�������o���s���ꍇ
	if (use_effect)
	{
		//���Ԓ�~�ƃJ�����V�F�C�N���s��
		time_manager.lock()->Start_Time_Slow(effect_stun.time_parameter.delay, effect_stun.time_parameter.time, effect_stun.time_parameter.speed);
		camera_controller.lock()->Shake_Camera(effect_stun.shake_parameter.count, effect_stun.shake_parameter.power);
	}
}

Vector3 Enemy_Manager::Get_Nearest_Enemy_Position(const Vector3& position)
{
	//�����̍��W�Ɉ�ԋ߂��G�l�~�[����������
	Vector3 nearest_position;
	float min_dis = FLT_MAX;
	for (const auto& enemy : enemy_list)
	{
		const Vector3 pos = enemy.lock()->transform->Get_Position();
		if (const float dis = Vector3::DistanceSquared(pos, position); dis < min_dis)
		{
			min_dis = dis;
			nearest_position = pos;
		}
	}
	nearest_position.y = position.y;
	return nearest_position;
}

bool Enemy_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"�U�����莞��", "##attack_check_time", &attack_check_time, window_center);
		ImGui::LeftText_DragFloat(u8"�_���[�W���A����", "##damaged_check_time", &damaged_check_time, window_center);

		if (ImGui::TreeNode(u8"�G���S�����o�p�����[�^"))
		{
			if (ImGui::TreeNode(u8"�X���[����"))
			{
				ImGui::LeftText_DragFloat(u8"�����f�B���C", "##effect_dead_time_parameter_delay", &effect_dead.time_parameter.delay, window_center);
				ImGui::LeftText_DragFloat(u8"���ʎ���", "##effect_dead_time_parameter_time", &effect_dead.time_parameter.time, window_center);
				ImGui::LeftText_DragFloat(u8"���x", "##effect_dead_time_parameter_speed", &effect_dead.time_parameter.speed, window_center);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"�J�����h��"))
			{
				ImGui::LeftText_DragInt(u8"��", "##effect_dead_shake_parameter_count", &effect_dead.shake_parameter.count, window_center);
				ImGui::LeftText_DragFloat(u8"���x", "##effect_dead_shake_parameter_power", &effect_dead.shake_parameter.power, window_center);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode(u8"�G�X�^�������o�p�����[�^"))
		{
			if (ImGui::TreeNode(u8"�X���[����"))
			{
				ImGui::LeftText_DragFloat(u8"�����f�B���C", "##effect_stun_time_parameter_delay", &effect_stun.time_parameter.delay, window_center);
				ImGui::LeftText_DragFloat(u8"���ʎ���", "##effect_stun_time_parameter_time", &effect_stun.time_parameter.time, window_center);
				ImGui::LeftText_DragFloat(u8"���x", "##effect_stun_time_parameter_speed", &effect_stun.time_parameter.speed, window_center);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"�J�����h��"))
			{
				ImGui::LeftText_DragInt(u8"��", "##effect_stun_shake_parameter_count", &effect_stun.shake_parameter.count, window_center);
				ImGui::LeftText_DragFloat(u8"���x", "##effect_stun_shake_parameter_power", &effect_stun.shake_parameter.power, window_center);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::Button(u8"�G_�ʏ�_01 ����"))
		{
			Instance_Enemy(Enemy_Type::Enemy_Normal_01, Vector3::Zero, Quaternion::Identity);
		}
	}
	return true;
}