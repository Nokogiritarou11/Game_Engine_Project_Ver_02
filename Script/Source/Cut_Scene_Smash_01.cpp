#include "Cut_Scene_Smash_01.h"

using namespace std;
using namespace BeastEngine;

void Cut_Scene_Smash_01::Awake()
{
	//�����o�|�C���^�̎擾
	camera_transform = GameObject::Find_With_Tag("main_camera").lock()->transform;
	root_transform = camera_transform.lock()->Get_Parent();
	player_transform = GameObject::Find_With_Tag("player").lock()->transform;
}

bool Cut_Scene_Smash_01::Play_Cut_Scene()
{
	//�^�C�}�[�̍X�V
	state_timer += Time::delta_time;

	const auto& root = root_transform.lock();
	const auto& p_trans = player_transform.lock();
	//�v���C���[��Ǐ]����
	root->Set_Local_Position(Vector3::Lerp(root->Get_Position(), p_trans->Get_Position() + Vector3(0, 1, 0), Time::delta_time * 30.0f));

	if (cut_state == 0)
	{
		//�J�b�g�V�[���J�n����
		const auto& data = state_data[0];
		const auto& camera = camera_transform.lock();
		const float ratio = Time::delta_time * 30.0f;

		//�}�ɃJ�����̎��_���؂�ւ��̂�h�����߁ALerp�ŏ����ʒu�ɃZ�b�g����
		root->Set_Local_Euler_Angles(Vector3::Lerp(root->Get_Euler_Angles(), p_trans->Get_Euler_Angles(), ratio));
		camera->Set_Local_Position(Vector3::Lerp(camera->Get_Local_Position(), data.start_position, ratio));
		camera->Set_Local_Euler_Angles(Vector3::Lerp(camera->Get_Local_Euler_Angles(), data.start_rotation, ratio));
		const Vector3 r = camera->Get_Euler_Angles();
		camera->Set_Euler_Angles(r.x, r.y, 0);

		//�X�e�[�g�̐؂�ւ�
		if (state_timer >= camera_init_time)
		{
			state_timer = 0;
			++cut_state;
		}
	}
	else if (cut_state == 1)
	{
		//1�ڂ̃J�����ړ��V�[�N�G���X
		const auto& data = state_data[0];
		const auto& camera = camera_transform.lock();
		const float ratio = state_timer / data.change_time;

		camera->Set_Local_Position(Vector3::Lerp(data.start_position, data.end_position, ratio));
		camera->Set_Local_Euler_Angles(Vector3::Lerp(data.start_rotation, data.end_rotation, ratio));
		const Vector3 r = camera->Get_Euler_Angles();
		camera->Set_Euler_Angles(r.x, r.y, 0);

		//�X�e�[�g�̐؂�ւ�
		if (state_timer >= data.change_time)
		{
			state_timer = 0;
			++cut_state;
		}
	}
	else if (cut_state == 2)
	{
		//2�ڂ̃J�����ړ��V�[�N�G���X
		const auto& data = state_data[1];
		const auto& camera = camera_transform.lock();
		const float ratio = state_timer / data.change_time;

		camera->Set_Local_Position(Vector3::Lerp(data.start_position, data.end_position, ratio));
		camera->Set_Local_Euler_Angles(Vector3::Lerp(data.start_rotation, data.end_rotation, ratio));
		const Vector3 r = camera->Get_Euler_Angles();
		camera->Set_Euler_Angles(r.x, r.y, 0);

		//�J�b�g�V�[���̏I��
		if (state_timer >= data.change_time)
		{
			state_timer = 0;
			cut_state = 0;
			return false;
		}
	}

	return true;
}

bool Cut_Scene_Smash_01::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Cut_Scene_Smash_01", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"�J�n���J�����ړ�����", "##camera_init_time", &camera_init_time, window_center);

		if (ImGui::TreeNode(u8"�J�����ړ��f�[�^"))
		{
			int size = static_cast<int>(state_data.size());

			ImGui::Text(u8"�v�f��");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				state_data.resize(size);
			}

			for (int i = 0; i < size; ++i)
			{
				ImGui::PushID(i);

				const string label = u8"�v�f " + to_string(i);
				if (ImGui::TreeNode(label.c_str()))
				{
					if (ImGui::TreeNode(u8"�J�n�p��"))
					{
						ImGui::LeftText_DragFloat3(u8"�ʒu", "##start_position", state_data[i].start_position, window_center);
						ImGui::LeftText_DragFloat3(u8"����", "##start_rotation", state_data[i].start_rotation, window_center);
						ImGui::TreePop();
					}
					if (ImGui::TreeNode(u8"�I���p��"))
					{
						ImGui::LeftText_DragFloat3(u8"�ʒu", "##end_position", state_data[i].end_position, window_center);
						ImGui::LeftText_DragFloat3(u8"����", "##end_rotation", state_data[i].end_rotation, window_center);
						ImGui::TreePop();
					}
					ImGui::LeftText_DragFloat(u8"�J�ڎ���", "##change_time", &state_data[i].change_time, window_center);

					ImGui::TreePop();
				}

				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}
	return true;
}