#include "Player_Input.h"
#include "Enemy_Manager.h"
#include "Player_Parameter.h"
#include "Player_Camera_Controller.h"

using namespace std;
using namespace BeastEngine;

void Player_Input::Awake()
{
	//�����o�|�C���^�̎擾
	animator = Get_Component<Animator>();
	parameter = Get_Component<Player_Parameter>();
	camera_transform = GameObject::Find_With_Tag("main_camera").lock()->transform;
	camera_controller = camera_transform.lock()->Get_Parent().lock()->Get_Component<Player_Camera_Controller>();
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();
}

void Player_Input::Set_State()
{
	const auto& anim = animator.lock();
	const auto& camera_trans = camera_transform.lock();
	const auto& param = parameter.lock();
	const auto& e_manager = enemy_manager.lock();

	// �J�����̕�������AX-Z���ʂ̒P�ʃx�N�g�����擾
	Vector3 camera_direction = camera_trans->Get_Forward();
	camera_direction.y = 0;
	camera_direction.Normalize();

	// �����L�[�̓��͒l�ƃJ�����̌�������A�ړ�����������
	const Vector2 axis = Input::Get_Pad_Axis_Left();
	(camera_direction * axis.y - camera_trans->Get_Left() * axis.x).Normalize(input_direction);

	//�ړ�����
	const bool stick_neutral = input_direction == Vector3::Zero;
	anim->Set_Bool("Move", !stick_neutral);

	//�K�[�h�p�ɃX�e�B�b�N�̃j���[�g�������m�F���ăt���O�����Z�b�g
	if (stick_neutral) is_stick_reset = true;
	if (is_stick_reset && !stick_neutral)
	{
		is_stick_reset = false;
		can_guard_stick = true;
		guard_stick_timer = guard_stick_time;
	}

	//��U��
	if (Input::Get_Pad_Button_Down(Button_Code::X))
	{
		anim->Set_Int("Attack_State", 1);
		can_guard_button = true;
		guard_button_timer = guard_button_time;
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

	//���b�N�I��
	if (Input::Get_Pad_Button_Down(Button_Code::Right_Shoulder))
	{
		if (!enemy_manager.lock()->enemy_list.empty())
		{
			param->camera_locking = !param->camera_locking;
		}
	}

	//�X�e�B�b�N���j���[�g������Ԃ���|���ꂽ����
	if (can_guard_stick)
	{
		//�^�C�}�[�̍X�V
		guard_stick_timer -= Time::delta_time;
		if (guard_stick_timer <= 0) can_guard_stick = false;
	}
	//��U���{�^���������ꂽ����
	if (can_guard_button)
	{
		//�^�C�}�[�̍X�V
		guard_button_timer -= Time::delta_time;
		if (guard_button_timer <= 0) can_guard_button = false;
	}

	//�K�[�h���͐�����
	if (can_guard_button && can_guard_stick && !param->guarding)
	{
		//�U�����̓G�����݂��Ă��邩
		if (const auto& attacking_list = e_manager->attacking_list; !attacking_list.empty())
		{
			for (const auto& enemy : attacking_list)
			{
				//�X�e�B�b�N��|���������ɍU�����̓G�����邩���m�F
				if (Vector3 target_pos = enemy.lock()->transform->Get_Position(); Get_Input_To_Target_Angle(target_pos) <= guard_degree)
				{
					//�K�[�h����
					anim->Set_Trigger("Guard");
					//�U������̕�������
					target_pos.y = transform->Get_Position().y;
					input_direction = Vector3::Zero;
					transform->Set_Local_Rotation(transform->Look_At(target_pos));
					break;
				}
			}
		}
		can_guard_button = false;
		can_guard_stick = false;
	}

	//����U��
	//�X�^�����̓G�����݂���ꍇ
	if (const auto& stunning_list = e_manager->stunning_list; !stunning_list.empty())
	{
		for (const auto& enemy : stunning_list)
		{
			//��苗���ȓ��ɑ��݂��邩�m�F
			if (const auto& enemy_lock = enemy.lock(); Vector3::DistanceSquared(transform->Get_Position(), enemy_lock->transform->Get_Position()) <= powf(2.5f, 2))
			{
				//�ҋ@��ԂŃ{�^���̓��͂������������`�F�b�N
				if (Input::Get_Pad_Button_Down(Button_Code::B) && param->is_ground && !param->attacking && !param->damaging && !param->guarding && !param->is_invincible)
				{
					//���݂�����������
					Vector3 pos = transform->Get_Position();
					Vector3 enemy_pos = enemy_lock->transform->Get_Position();
					enemy_pos.y = pos.y;
					pos.y = enemy_pos.y;
					transform->Set_Local_Rotation(transform->Look_At(enemy_pos));
					enemy_lock->transform->Set_Local_Rotation(enemy_lock->transform->Look_At(pos));
					enemy_lock->transform->Set_Local_Position(pos + transform->Get_Forward() * 2.0f);

					//�G�̃A�j���[�V�������Đ����A���S������
					enemy_lock->Get_Component<Collider>()->Set_Layer(5);
					enemy_lock->Get_Component<Animator>()->Set_Trigger("Smash");
					enemy_manager.lock()->Enemy_Dead(false, enemy);

					//�J�b�g�V�[�����Đ�
					camera_controller.lock()->Play_Cut_Scene(static_cast<int>(enemy_lock->type));
					anim->Set_Int("Smash_Number", static_cast<int>(enemy_lock->type));
					input_direction = Vector3::Zero;
				}
				break;
			}
		}
	}
}

float Player_Input::Get_Input_To_Target_Angle(Vector3 target_position) const
{
	//�����̍��W�̓��͕����ɑ΂��Ă̊p�x�����Z�o����
	const Vector3 player_pos = transform->Get_Position();
	target_position.y = player_pos.y;
	Vector3 target_vector = target_position - (player_pos - input_direction * 1.5f);
	target_vector.Normalize();
	return acosf(input_direction.Dot(target_vector)) * 180.0f / static_cast<float>(M_PI);
}

bool Player_Input::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Player_Input", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_DragFloat(u8"�e���\�p�x", "##Guard_Degree", &guard_degree, window_center);
		ImGui::LeftText_DragFloat(u8"�K�[�h���莞��_�X�e�B�b�N", "##Guard_Stick_Time", &guard_stick_time, window_center);
		ImGui::LeftText_DragFloat(u8"�K�[�h���莞��_�{�^��", "##Guard_Button_Time", &guard_button_time, window_center);
	}
	return true;
}