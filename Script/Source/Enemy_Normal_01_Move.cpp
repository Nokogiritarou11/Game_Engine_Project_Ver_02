#include "Enemy_Normal_01_Move.h"
#include "Enemy_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Normal_01_Move::Awake()
{
	//�����o�|�C���^�̎擾
	rigidbody = Get_Component<Capsule_Collider>()->rigidbody;
	animator = Get_Component<Animator>();
	parameter = Get_Component<Enemy_Parameter>();
	target_transform = GameObject::Find_With_Tag("player").lock()->transform;
}

void Enemy_Normal_01_Move::Move_Normal()
{
	const auto& target = target_transform.lock();
	Vector3 target_position = target->Get_Position();
	target_position.y = transform->Get_Position().y;
	//�v���C���[�����֊��炩�ɕ����]��
	transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Local_Rotation(), transform->Look_At(target_position), turn_speed * Time::delta_time));

	const auto& rb = rigidbody.lock();
	Vector3 speed;
	//�ړ���Ԏ�
	if (const auto& param = parameter.lock(); param->moving)
	{
		//�ړ������ɂ���đ��x�������ω�
		switch (param->move_state)
		{
			case Move_State::Forward:
				speed = transform->Get_Forward() * run_speed * Time::delta_time;
				break;
			case Move_State::Right:
				speed = -transform->Get_Left() * run_speed * 0.25f * Time::delta_time;
				break;
			case Move_State::Left:
				speed = transform->Get_Left() * run_speed * 0.25f * Time::delta_time;
				break;
		}
		//�d�͏���
		speed.y = y_axis_velocity;
	}
	else
	{
		//�d�͏���
		speed = Vector3(0, y_axis_velocity, 0);
	}
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Attack()
{
	//�U���O���[�V�������̏ꍇ
	if (const auto& param = parameter.lock(); param->is_attack_preliminary)
	{
		const auto& target = target_transform.lock();
		Vector3 target_position = target->Get_Position();
		target_position.y = transform->Get_Position().y;
		//�v���C���[�����֊��炩�ɕ����]��
		transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Local_Rotation(), transform->Look_At(target_position), turn_speed * Time::delta_time));
	}
	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	//�d�͏���
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Damage()
{
	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	//�d�͏���
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Guard()
{
	const auto& rb = rigidbody.lock();
	Vector3 speed = transform->Get_Forward() * move_speed * Time::delta_time;
	//�d�͏���
	speed.y = y_axis_velocity;
	rb->Set_Velocity(speed);
}

void Enemy_Normal_01_Move::Move_Update()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	//�A�j���[�V��������ړ����x�𓾂�
	move_speed = anim->Get_Float("Move_Speed");

	if (param->is_ground)
	{
		//�n��ɂ���ꍇ
		//Animator����W�����v���邩�𔻒f
		if (anim->Get_Bool("Add_Jump_Force"))
		{
			y_axis_velocity = jump_power;
			anim->Set_Bool("Add_Jump_Force", false);
		}
	}
	else
	{
		//�󒆂ɂ���ꍇ
		//�󒆍U���Ȃǂŋ󒆂ɗ��܂邩��Animator���画�f
		if (const bool stay_air = anim->Get_Bool("Stay_Air"); is_stay_air != stay_air)
		{
			//�d�͂�؂�
			is_stay_air = stay_air;
			if (is_stay_air)
			{
				y_axis_velocity = 0;
			}
		}

		//�d�͂����Z����
		if (!is_stay_air)
		{
			y_axis_velocity -= down_power * Time::delta_time;
		}
	}
}

bool Enemy_Normal_01_Move::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Normal_01_Move", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"�ړ����x", "##Run_Speed", &run_speed, window_center);
		ImGui::LeftText_DragFloat(u8"��]���x", "##Turn_Speed", &turn_speed, window_center);
		ImGui::LeftText_DragFloat(u8"�W�����v�̋���", "##Jump_Power", &jump_power, window_center);
		ImGui::LeftText_DragFloat(u8"�������x", "##Down_Power", &down_power, window_center);
	}
	return true;
}