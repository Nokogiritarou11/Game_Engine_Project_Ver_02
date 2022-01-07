#include "Player_Camera_Controller.h"
#include "Player_Parameter.h"
#include "Enemy_Manager.h"
#include "Interface_Cut_Scene.h"

using namespace std;
using namespace BeastEngine;

void Player_Camera_Controller::Awake()
{
	//�����o�|�C���^�̎擾
	const auto& obj = GameObject::Find_With_Tag("player").lock();
	player_transform = obj->transform;
	parameter = obj->Get_Component<Player_Parameter>();
	camera_transform = GameObject::Find_With_Tag("main_camera").lock()->transform;
	camera = camera_transform.lock()->Get_Component<Camera>();
	enemy_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Enemy_Manager>();

	//�����ʒu��ۑ�����
	final_position = camera_transform.lock()->Get_Local_Position();

	//�g�p����J�b�g�V�[����o�^����
	cut_scene.resize(3);
	cut_scene[0] = transform->Find("Cut_Scene_Smash_01").lock()->Get_Component<Interface_Cut_Scene>();
	cut_scene[2] = transform->Find("Cut_Scene_Smash_03").lock()->Get_Component<Interface_Cut_Scene>();
}

void Player_Camera_Controller::LateUpdate()
{
	const auto& e_manager = enemy_manager.lock();
	const auto& param = parameter.lock();

	if (is_playing_cut_scene)
	{
		//�J�b�g�V�[���Đ����̏ꍇ
		//�J�b�g�V�[���̍Đ����I������܂őҋ@
		const auto& cut = playing_cut_scene.lock();
		if (cut->is_end_play)
		{
			//�Đ����I�������̂ŃJ�����̈ʒu��o�^����
			final_position = camera_transform.lock()->Get_Local_Position();
			is_playing_cut_scene = false;
		}
		else
		{
			final_position = cut->Play_Cut_Scene();
		}
	}
	else
	{
		if (e_manager->enemy_list.empty())
		{
			//�G�����Ȃ��ꍇ
			Update_Free_Look();
		}
		else
		{
			if (param->camera_locking && !param->target.expired())
			{
				//���b�N�I����
				Update_Lock_On();
			}
			else
			{
				//�񃍃b�N�I�����퓬��
				Update_Battle();
			}
		}
	}

	//�J�����V�F�C�N�񐔂��c���Ă���ꍇ
	if (shake_count > 0)
	{
		//�^�C�}�[�̍X�V
		shake_timer += Time::delta_time;

		//60fps��ŃJ�����V�F�C�N���s��
		if (constexpr float frame_time = 1.0f / 60.0f; shake_timer <= frame_time)
		{
			//�J�����V�F�C�N����
			const auto& camera_trans = camera_transform.lock();
			const Vector3 pos = final_position;
			const float x = pos.x + Mathf::Random_Range(-1.0f, 1.0f) * shake_power;
			const float y = pos.y + Mathf::Random_Range(-1.0f, 1.0f) * shake_power;
			camera_trans->Set_Local_Position(x, y, pos.z);
			shake_timer = 0;
			//�c��̃V�F�C�N�񐔂����炷
			--shake_count;
		}
	}
}

void Player_Camera_Controller::Update_Free_Look()
{
	const auto& p_trans = player_transform.lock();
	const auto& param = parameter.lock();

	//�e
	{
		//�ړ����͂��擾
		const Vector2 left_axis = Input::Get_Pad_Axis_Left();

		const Vector3 now_pos = transform->Get_Position();
		Vector3 follow_pos = p_trans->Get_Position();
		follow_pos.y += 1;

		//�v���C���[�ւ̒Ǐ]����
		//�ړ����͂ɂ���ĒǏ]���x��ς���
		follow_pos.x = Mathf::Lerp(now_pos.x, follow_pos.x, follow_speed * (1 + abs(left_axis.y) * 0.75f) * Time::delta_time);
		follow_pos.y = Mathf::Lerp(now_pos.y, follow_pos.y, follow_speed * 0.75f * Time::delta_time);
		follow_pos.z = Mathf::Lerp(now_pos.z, follow_pos.z, follow_speed * (1 + abs(left_axis.y) * 0.75f) * Time::delta_time);

		transform->Set_Local_Position(follow_pos);

		//�J�����̉�]����
		const Vector2 axis = Input::Get_Pad_Axis_Right();
		Vector3 angle = { -axis.y * rotate_speed * Time::delta_time, -axis.x * rotate_speed * Time::delta_time, 0 };

		//�ړ������ɂ���ăJ������������]������
		if (param->moving)
		{
			angle.y -= left_axis.x * rotate_speed * 0.25f * Time::delta_time;
		}

		const Vector3 rot = transform->Get_Euler_Angles() + angle;

		//�ő�p�x�ɃN�����v
		const float angle_x = 180.0f <= rot.x ? rot.x - 360 : rot.x;
		transform->Set_Local_Euler_Angles(Mathf::Clamp(angle_x, angle_limit_down, angle_limit_up), rot.y, rot.z);
	}

	//�J�����{��
	{
		//�w��ʒu�֊��炩�Ɉړ�������
		const auto& camera_trans = camera_transform.lock();
		camera_trans->Set_Local_Position(Vector3::Lerp(final_position, default_position, Time::delta_time));
		camera_trans->Set_Local_Euler_Angles(Vector3::Lerp(camera_trans->Get_Local_Euler_Angles(), default_rotation, Time::delta_time));
		const Vector3 r = camera_trans->Get_Euler_Angles();
		camera_trans->Set_Euler_Angles(r.x, r.y, 0);

		final_position = camera_trans->Get_Local_Position();
	}
}

void Player_Camera_Controller::Update_Battle()
{
	const auto& param = parameter.lock();
	const auto& e_manager = enemy_manager.lock();
	const auto& p_trans = player_transform.lock();
	const auto& camera_trans = camera_transform.lock();
	const auto& target = e_manager->last_attack_target.lock();

	//�ŏI�U���Ώۂ����b�N�I���Ώۂɐݒ�
	param->target = target;

	const int enemy_count = e_manager->enemy_list.size();
	const Vector2 target_view_pos = camera.lock()->World_To_Viewport_Point(target->transform->Get_Position());

	//�e
	{
		//�v���C���[�ւ̒Ǐ]����
		const Vector3 now_pos = transform->Get_Position();
		Vector3 follow_pos = p_trans->Get_Position();
		follow_pos.y += 1;

		follow_pos.x = Mathf::Lerp(now_pos.x, follow_pos.x, follow_speed * Time::delta_time);
		follow_pos.y = Mathf::Lerp(now_pos.y, follow_pos.y, follow_speed * 0.75f * Time::delta_time);
		follow_pos.z = Mathf::Lerp(now_pos.z, follow_pos.z, follow_speed * Time::delta_time);

		transform->Set_Local_Position(follow_pos);

		//�J�����̉�]����
		const Vector2 axis = Input::Get_Pad_Axis_Right();
		Vector3 angle = { -axis.y * rotate_speed * Time::delta_time, -axis.x * rotate_speed * Time::delta_time, 0 };

		if (param->moving || param->attacking)
		{
			// �J�����̕�������AX-Z���ʂ̒P�ʃx�N�g�����擾
			Vector3 camera_direction = camera_trans->Get_Forward();
			camera_direction.y = 0;
			camera_direction.Normalize();

			const Vector3 axis_camera = camera_direction.Cross(p_trans->Get_Forward());
			const float angle_camera = acosf(camera_direction.Dot(p_trans->Get_Forward())) * 180.0f / static_cast<float>(M_PI) * (axis_camera.y < 0 ? -1 : 1) / 180.0f;
			if (angle_camera >= 0.4f)
			{
				angle.y += (angle_camera - 0.4f) * rotate_speed * 0.5f * Time::delta_time;
			}
			else if (angle_camera <= -0.4f)
			{
				angle.y += (angle_camera + 0.4f) * rotate_speed * 0.5f * Time::delta_time;
			}
		}
		const Vector3 rot = transform->Get_Euler_Angles() + angle;

		//�ő�p�x�ɃN�����v
		const float angle_x = 180.0f <= rot.x ? rot.x - 360 : rot.x;
		transform->Set_Local_Euler_Angles(Mathf::Clamp(angle_x, angle_limit_down, angle_limit_up), rot.y, rot.z);
	}

	//�J�����{��
	{
		Vector3 pos = battle_position;
		Vector3 rot = battle_rotation;

		if (is_target_right)
		{
			//���~�Ώۂ���ʉE���ɂ���ꍇ
			//���~�Ώۂ����ɍs������؂�ւ�
			if (target_view_pos.x < 0.425f)
			{
				is_target_right = false;
			}
		}
		else
		{
			//���~�Ώۂ���ʍ����ɂ���ꍇ
			//���~�Ώۂ��E�ɍs������؂�ւ�
			if (target_view_pos.x > 0.575f)
			{
				is_target_right = true;
			}
			//�p�������E���]
			pos.x *= -1;
			rot.y *= -1;
		}

		//�v���C���[���󒆂ɂ���ꍇ�͏����J����������
		if (!param->is_ground)
		{
			pos.z -= 0.75f;
		}

		//�G�̐��ɂ���ăJ����������
		pos.z -= 0.5f * (Mathf::Clamp(static_cast<float>(enemy_count), 0, 4) - 1);

		camera_trans->Set_Local_Position(Vector3::Lerp(final_position, pos, Time::delta_time * 2.0f));

		camera_trans->Set_Local_Euler_Angles(Vector3::Lerp(camera_trans->Get_Local_Euler_Angles(), rot, Time::delta_time * 2.0f));
		const Vector3 r = camera_trans->Get_Euler_Angles();
		camera_trans->Set_Euler_Angles(r.x, r.y, 0);

		final_position = camera_trans->Get_Local_Position();
	}
}

void Player_Camera_Controller::Update_Lock_On()
{
	const auto& param = parameter.lock();
	const auto& p_trans = player_transform.lock();
	const auto& camera_trans = camera_transform.lock();
	const auto& target = param->target.lock()->lock_on_target.lock();

	const Vector2 target_view_pos = camera.lock()->World_To_Viewport_Point(target->Get_Position());

	//�e
	{
		//�v���C���[�ւ̒Ǐ]����
		const Vector3 now_pos = transform->Get_Position();
		Vector3 follow_pos = p_trans->Get_Position();
		follow_pos.y += 1;

		follow_pos.x = Mathf::Lerp(now_pos.x, follow_pos.x, follow_speed * Time::delta_time);
		follow_pos.y = Mathf::Lerp(now_pos.y, follow_pos.y, follow_speed * 0.75f * Time::delta_time);
		follow_pos.z = Mathf::Lerp(now_pos.z, follow_pos.z, follow_speed * Time::delta_time);

		transform->Set_Local_Position(follow_pos);

		//�J�����̉�]����
		transform->Set_Local_Rotation(Quaternion::Slerp(transform->Get_Rotation(), transform->Look_At(target->Get_Position()), lock_on_targeting_speed * Time::delta_time));
	}

	//�J�����{��
	{
		Vector3 pos = lock_position;
		Vector3 rot = lock_rotation;

		if (is_target_right)
		{
			//���~�Ώۂ���ʉE���ɂ���ꍇ
			//���~�Ώۂ����ɍs������؂�ւ�
			if (target_view_pos.x < 0.35f)
			{
				is_target_right = false;
			}
		}
		else
		{
			//���~�Ώۂ���ʍ����ɂ���ꍇ
			//���~�Ώۂ��E�ɍs������؂�ւ�
			if (target_view_pos.x > 0.65f)
			{
				is_target_right = true;
			}
			//�p�������E���]
			pos.x *= -1;
			rot.y *= -1;
		}

		//�v���C���[���󒆂ɂ���ꍇ�͏����J����������
		if (!param->is_ground)
		{
			pos.z -= 0.75f;
		}

		camera_trans->Set_Local_Position(Vector3::Lerp(final_position, pos, Time::delta_time * 2.0f));

		camera_trans->Set_Local_Euler_Angles(Vector3::Lerp(camera_trans->Get_Local_Euler_Angles(), rot, Time::delta_time * 2.0f));
		const Vector3 r = camera_trans->Get_Euler_Angles();
		camera_trans->Set_Euler_Angles(r.x, r.y, 0);

		final_position = camera_trans->Get_Local_Position();
	}
}

void Player_Camera_Controller::Shake_Camera(const int& count, const float& power)
{
	if (count > 0)
	{
		//�J�����V�F�C�N�񐔂�ǉ�����
		shake_count = count;
		shake_power = power;
		shake_timer = 0;
	}
}

void Player_Camera_Controller::Play_Cut_Scene(const int& index)
{
	if (index < static_cast<int>(cut_scene.size()))
	{
		//�J�b�g�V�[���̍Đ�
		playing_cut_scene = cut_scene[index];
		playing_cut_scene.lock()->is_end_play = false;
		is_playing_cut_scene = true;
	}
}


bool Player_Camera_Controller::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Player_Camera_Controller", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"������p�x�ő�", "##Angle_Limit_Up", &angle_limit_up, window_center);
		ImGui::LeftText_DragFloat(u8"�������p�x�ő�", "##Angle_Limit_Down", &angle_limit_down, window_center);
		ImGui::LeftText_DragFloat(u8"��]���x", "##Rotate_Speed", &rotate_speed, window_center);
		ImGui::LeftText_DragFloat(u8"�Ǐ]���x", "##Follow_Speed", &follow_speed, window_center);
		ImGui::LeftText_DragFloat(u8"���b�N������Ǐ]���x", "##Lock_On_Targeting_Speed", &lock_on_targeting_speed, window_center);

		if (ImGui::TreeNode(u8"���펞�J�����p��"))
		{
			ImGui::LeftText_DragFloat3(u8"�ʒu", "##Default_Position", default_position, window_center);
			ImGui::LeftText_DragFloat3(u8"����", "##Default_Rotation", default_rotation, window_center);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(u8"�퓬���J�����p��"))
		{
			ImGui::LeftText_DragFloat3(u8"�ʒu", "##Battle_Position", battle_position, window_center);
			ImGui::LeftText_DragFloat3(u8"����", "##Battle_Rotation", battle_rotation, window_center);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode(u8"���b�N�I�����J�����p��"))
		{
			ImGui::LeftText_DragFloat3(u8"�ʒu", "##Lock_Position", lock_position, window_center);
			ImGui::LeftText_DragFloat3(u8"����", "##Lock_Rotation", lock_rotation, window_center);
			ImGui::TreePop();
		}
	}
	return true;
}